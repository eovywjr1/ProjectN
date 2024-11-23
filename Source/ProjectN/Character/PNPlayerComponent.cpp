// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PNPlayerComponent.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PNEnhancedInputComponent.h"
#include "PNGameplayTags.h"
#include "PNPawnComponent.h"
#include "PNPawnData.h"
#include "GameFramework/Character.h"
#include "Input/PNInputConfig.h"
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
				PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Input_Jumping);
				PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Completed, this, &ThisClass::Input_StopJumping);
				PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
			}
		}
	}
}

void UPNPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn* Owner = GetPawn<APawn>();
	APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	APNPlayerState* PlayerState = Owner->GetPlayerState<APNPlayerState>();
	UAbilitySystemComponent* ASComponent = PlayerState->GetAbilitySystemComponent();
	ASComponent->AddLooseGameplayTag(FPNGameplayTags::FindTagByString("Status.Peace"));
}

void UPNPlayerComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (AController* Controller = Pawn->GetController())
	{
		const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		Pawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		Pawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void UPNPlayerComponent::Input_Jumping(const FInputActionValue& InputActionValue)
{
	GetPawnChecked<ACharacter>()->Jump();
}

void UPNPlayerComponent::Input_StopJumping(const FInputActionValue& InputActionValue)
{
	GetPawnChecked<ACharacter>()->StopJumping();
}

void UPNPlayerComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (AController* Controller = Pawn->GetController())
	{
		const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

		Pawn->AddControllerYawInput(LookAxisVector.X);
		Pawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}
