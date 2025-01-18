// Fill out your copyright notice in the Description page of Project Settings.


#include "PNPlayerInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "PNEnhancedInputComponent.h"
#include "PNGameplayTags.h"
#include "PNPawnComponent.h"
#include "PNPawnData.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "Actor/PNCharacterPlayer.h"
#include "Input/PNInputConfig.h"
#include "InputMappingContext.h"
#include "Player/PNPlayerController.h"

UPNPlayerInputComponent::UPNPlayerInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ControlMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ProjectN/Input/IMC_Control.IMC_Control'"));
	if (ControlMappingContextRef.Object)
	{
		ControlMappingContext = ControlMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> CameraMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ProjectN/Input/IMC_Camera.IMC_Camera'"));
	if (CameraMappingContextRef.Object)
	{
		CameraMappingContext = CameraMappingContextRef.Object;
	}
}

void UPNPlayerInputComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	if (PlayerInputComponent == nullptr)
	{
		return;
	}

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController->IsLocalController())
	{
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())->AddMappingContext(CameraMappingContext, 0);
		EnableControlInput(true);
	}

	APawn* Owner = GetPawn<APawn>();
	const UPNPawnComponent* PawnComponent = Owner->FindComponentByClass<UPNPawnComponent>();
	const UPNPawnData* PawnData = PawnComponent->GetPawnData();
	check(PawnData);
	const UPNInputConfig* InputConfig = PawnData->InputConfig;
	check(InputConfig);

	const FPNGameplayTags& GameplayTags = FPNGameplayTags::Get();
	UPNEnhancedInputComponent* PNEnhancedInputComponent = Cast<UPNEnhancedInputComponent>(PlayerInputComponent);

	PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_LockOn, ETriggerEvent::Triggered, this, &ThisClass::Input_LockOn);
	PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_NextLockOnTarget, ETriggerEvent::Triggered, this, &ThisClass::Input_NextLockOnTarget);

	PNEnhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityPressed, &ThisClass::Input_AbilityReleased);
}

void UPNPlayerInputComponent::EnableControlInput(bool bEnable) const
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController == nullptr)
	{
		return;
	}

	if (!PlayerController->IsLocalController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	if (bEnable)
	{
		Subsystem->AddMappingContext(ControlMappingContext, 0);
	}
	else
	{
		Subsystem->RemoveMappingContext(ControlMappingContext);
	}
}

void UPNPlayerInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APNCharacterPlayer* Owner = GetPawn<APNCharacterPlayer>();
	check(Owner);

	Owner->MoveByInput(InputActionValue.Get<FVector2D>());

	LastMovementInput = InputActionValue.Get<FVector2D>();
}

void UPNPlayerInputComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	GetController<APNPlayerController>()->RotationByInput(InputActionValue.Get<FVector2D>());
}

void UPNPlayerInputComponent::Input_NextLockOnTarget(const FInputActionValue& InputActionValue)
{
	if (bIsActivatedLockOn == false)
	{
		return;
	}

	GetController<APNPlayerController>()->SetNextPriorityLockOnTargetActor();
}

void UPNPlayerInputComponent::Input_LockOn(const FInputActionValue& InputActionValue)
{
	bIsActivatedLockOn = !bIsActivatedLockOn;

	if (bIsActivatedLockOn)
	{
		GetController<APNPlayerController>()->ActivateLockOn();
	}
	else
	{
		GetController<APNPlayerController>()->DeActivateLockOn();
	}
}

void UPNPlayerInputComponent::Input_AbilityPressed(FGameplayTag InputTag)
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

void UPNPlayerInputComponent::Input_AbilityReleased(FGameplayTag InputTag)
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
