// Copyright Epic Games, Inc. All Rights Reserved.

#include "PNCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PNEnhancedInputComponent.h"
#include "PNPawnComponent.h"
#include "PNPlayerComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Player/PNPlayerState.h"

//////////////////////////////////////////////////////////////////////////

APNCharacter::APNCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	/////////////////////////////////////////////////////////////////////////////////////////

	OverrideInputComponentClass = UPNEnhancedInputComponent::StaticClass();

	PawnComponent = CreateDefaultSubobject<UPNPawnComponent>(TEXT("PNPawnComponent"));
}

UAbilitySystemComponent* APNCharacter::GetAbilitySystemComponent() const
{
	return PawnComponent->GetAbilitySystemComponent();
}

void APNCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APNPlayerState* PNPlayerState = GetPlayerState<APNPlayerState>())
	{
		// Todo. 캐릭터 스폰할 때 AbilitySystemComponent 초기화해줘야 함
		UAbilitySystemComponent* ASComponent = PNPlayerState->GetAbilitySystemComponent();
		PawnComponent->SetAbilitySystemComponent(ASComponent);
		ASComponent->InitAbilityActorInfo(PNPlayerState, this);
	
		// int32 InputID = 0;
	
		// for (const TSubclassOf<UGameplayAbility>& Ability : Abilities)
		// {
		// 	FGameplayAbilitySpec AbilitySpec(Ability);
		// 	AbilitySpec.InputID = InputID;
		// 	++InputID;
		//
		// 	AbilitySystemComponent->GiveAbility(AbilitySpec);
		// }
	}
}

void APNCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// 이 시점에서 InputComponent가 유효함
	if (UPNPlayerComponent* PlayerComponent = FindComponentByClass<UPNPlayerComponent>())
	{
		PlayerComponent->InitializePlayerInput(PlayerInputComponent);
	}
}
