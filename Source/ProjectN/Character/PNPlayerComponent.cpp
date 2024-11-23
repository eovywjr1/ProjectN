// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PNPlayerComponent.h"

#include "EnhancedInputSubsystems.h"
#include "PNEnhancedInputComponent.h"
#include "PNGameplayTags.h"
#include "PNPawnComponent.h"
#include "PNPawnData.h"
#include "Input/PNInputConfig.h"

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
			}
		}
	}
}

void UPNPlayerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* Pawn = GetPawn<APawn>();

	if (APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
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
