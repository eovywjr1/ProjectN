// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PNPlayerComponent.h"

#include "EnhancedInputSubsystems.h"
#include "PNEnhancedInputComponent.h"
#include "PNGameplayTags.h"
#include "PNPawnComponent.h"
#include "PNPawnData.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "Input/PNInputConfig.h"
#include "Player/PNPlayerController.h"
#include "Player/PNPlayerState.h"

UPNPlayerComponent::UPNPlayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UPNPlayerComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);

	if (const UPNPawnComponent* PawnComponent = Pawn->FindComponentByClass<UPNPawnComponent>())
	{
		if (const UPNPawnData* PawnData = PawnComponent->GetPawnData())
		{
			if (const UPNInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FPNGameplayTags& GameplayTags = FPNGameplayTags::Get();

				UPNEnhancedInputComponent* PNEnhancedInputComponent = CastChecked<UPNEnhancedInputComponent>(PlayerInputComponent);

				PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
				PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
				PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_LockOn, ETriggerEvent::Triggered, this, &ThisClass::Input_LockOn);
				PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_EnableLockOn, ETriggerEvent::Triggered, this, &ThisClass::Input_EnableLockOn);

				PNEnhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityPressed, &ThisClass::Input_AbilityReleased);
			}
		}
	}
}

void UPNPlayerComponent::EnableInput(bool bIsEnable) const
{
	APawn* Owner = GetPawnChecked<APawn>();
	APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
	if (PlayerController == nullptr)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (Subsystem == nullptr)
	{
		return;
	}

	if (bIsEnable)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		Subsystem->AddMappingContext(CameraMappingContext, 0);
	}
	else
	{
		Subsystem->RemoveMappingContext(DefaultMappingContext);
	}
}

void UPNPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn* Owner = GetPawn<APawn>();
	if (Owner == nullptr)
	{
		return;
	}

	EnableInput(true);

	APNPlayerState* PlayerState = Owner->GetPlayerState<APNPlayerState>();
	UAbilitySystemComponent* ASComponent = PlayerState->GetAbilitySystemComponent();
	ASComponent->AddLooseGameplayTag(FPNGameplayTags::Get().Status_Peace);

#ifdef WITH_EDITOR
	// 테스트 용도
	ASComponent->AddLooseGameplayTag(FPNGameplayTags::Get().Status_Fight);
#endif
}

void UPNPlayerComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (Pawn == nullptr)
	{
		return;
	}

	AController* Controller = Pawn->GetController();
	if (Controller == nullptr)
	{
		return;
	}

	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	Pawn->AddMovementInput(ForwardDirection, MovementVector.Y);
	Pawn->AddMovementInput(RightDirection, MovementVector.X);

	LastMovementInput = MovementVector;
}

void UPNPlayerComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	APawn* Owner = GetPawnChecked<APawn>();
	APNPlayerController* PlayerController = CastChecked<APNPlayerController>(Owner->GetController());
	if (PlayerController->CanCameraInputControl() == false)
	{
		return;
	}

	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	Owner->AddControllerYawInput(LookAxisVector.X);
	Owner->AddControllerPitchInput(LookAxisVector.Y);
}

void UPNPlayerComponent::Input_EnableLockOn(const FInputActionValue& InputActionValue)
{
	bIsEnableLockOn = !bIsEnableLockOn;

	if (bIsEnableLockOn == false)
	{
		APawn* Owner = GetPawnChecked<APawn>();
		APNPlayerController* PlayerController = CastChecked<APNPlayerController>(Owner->GetController());
		PlayerController->DisableLockOn();
	}
}

void UPNPlayerComponent::Input_LockOn(const FInputActionValue& InputActionValue)
{
	if (bIsEnableLockOn == false)
	{
		return;
	}

	APawn* Owner = GetPawnChecked<APawn>();
	APNPlayerController* PlayerController = CastChecked<APNPlayerController>(Owner->GetController());
	PlayerController->SetLockOnTargetActor();
}

void UPNPlayerComponent::Input_AbilityPressed(FGameplayTag InputTag)
{
	APawn* Owner = GetPawn<APawn>();
	if (Owner == nullptr)
	{
		return;
	}

	const UPNPawnComponent* PawnComponent = Owner->FindComponentByClass<UPNPawnComponent>();
	if (PawnComponent == nullptr)
	{
		return;
	}

	UPNAbilitySystemComponent* ASC = PawnComponent->GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}

	ASC->AbilityInputPressed(InputTag);
}

void UPNPlayerComponent::Input_AbilityReleased(FGameplayTag InputTag)
{
	APawn* Owner = GetPawn<APawn>();
	if (Owner == nullptr)
	{
		return;
	}

	const UPNPawnComponent* PawnComponent = Owner->FindComponentByClass<UPNPawnComponent>();
	if (PawnComponent == nullptr)
	{
		return;
	}

	UPNAbilitySystemComponent* ASC = PawnComponent->GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}

	ASC->AbilityInputReleased(InputTag);
}
