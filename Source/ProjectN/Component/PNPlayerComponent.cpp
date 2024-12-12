// Fill out your copyright notice in the Description page of Project Settings.


#include "PNPlayerComponent.h"

#include "EnhancedInputSubsystems.h"
#include "GameplayTagsManager.h"
#include "PNEnhancedInputComponent.h"
#include "PNGameplayTags.h"
#include "PNPawnComponent.h"
#include "PNPawnData.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "Actor/PNCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
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

	ACharacter* Owner = GetPawn<ACharacter>();
	if (Owner == nullptr)
	{
		return;
	}

	EnableInput(true);

	UAbilitySystemComponent* AbilitySystemComponent = Owner->GetPlayerState<APNPlayerState>()->GetAbilitySystemComponent();
	AbilitySystemComponent->AddLooseGameplayTag(FPNGameplayTags::Get().Status_Peace);
	AbilitySystemComponent->AddLooseGameplayTag(FPNGameplayTags::Get().Status_Idle);

	FGameplayTagContainer ActionTagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(FPNGameplayTags::Get().Action);
	OnActionTagDelegateHandle = AbilitySystemComponent->RegisterGenericGameplayTagEvent().AddUObject(this, &ThisClass::OnUpdateActionTag);

	Owner->OnCharacterMovementUpdated.AddDynamic(this, &ThisClass::OnMovementUpdated);

#ifdef WITH_EDITOR
	// 테스트 용도
	AbilitySystemComponent->AddLooseGameplayTag(FPNGameplayTags::Get().Status_Fight);
#endif
}

void UPNPlayerComponent::DestroyComponent(bool bPromoteChildren)
{
	if (APNCharacter* Owner = GetPawn<APNCharacter>())
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = Owner->GetAbilitySystemComponent())
		{
			AbilitySystemComponent->RegisterGenericGameplayTagEvent().Remove(OnActionTagDelegateHandle);
		}
	}

	Super::DestroyComponent(bPromoteChildren);
}

void UPNPlayerComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APNCharacter* Owner = GetPawn<APNCharacter>();
	if (Owner == nullptr)
	{
		return;
	}

	AController* Controller = Owner->GetController();
	if (Controller == nullptr)
	{
		return;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = Owner->GetAbilitySystemComponent())
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FPNGameplayTags::Get().Action_Move, 1);
	}

	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	Owner->AddMovementInput(ForwardDirection, MovementVector.Y);
	Owner->AddMovementInput(RightDirection, MovementVector.X);

	LastMovementInput = MovementVector;
}

void UPNPlayerComponent::OnMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	APNCharacter* Owner = GetPawn<APNCharacter>();
	if (Owner == nullptr)
	{
		return;
	}

	if (UPawnMovementComponent* MovementComponent = Owner->GetMovementComponent())
	{
		if (MovementComponent->Velocity.IsNearlyZero())
		{
			if (UAbilitySystemComponent* AbilitySystemComponent = Owner->GetAbilitySystemComponent())
			{
				AbilitySystemComponent->SetLooseGameplayTagCount(FPNGameplayTags::Get().Action_Move, 0);
			}
		}
	}
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

void UPNPlayerComponent::OnUpdateActionTag(const FGameplayTag GameplayTag, int32 Count) const
{
	if (GameplayTag.MatchesTag(FPNGameplayTags::Get().Action) == false)
	{
		return;
	}

	APNCharacter* Owner = GetPawn<APNCharacter>();
	if (Owner == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = Owner->GetAbilitySystemComponent();
	check(AbilitySystemComponent);

	if (Count > 0)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FPNGameplayTags::Get().Status_Idle, 0);
	}
	else if (Owner->IsIdle())
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FPNGameplayTags::Get().Status_Idle, 1);
	}
}
