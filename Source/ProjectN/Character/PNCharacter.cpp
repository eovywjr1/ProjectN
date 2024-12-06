// Copyright Epic Games, Inc. All Rights Reserved.

#include "PNCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "PNEnhancedInputComponent.h"
#include "PNPawnComponent.h"
#include "PNPawnData.h"
#include "PNPlayerComponent.h"
#include "PNStatusActorComponent.h"
#include "AbilitySystem/PNAbilitySet.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/PNWeaponAttributeSet.h"
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
	CreateDefaultSubobject<UPNStatusActorComponent>(TEXT("StatusActorComponent"));
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
		// Todo. 캐릭터 스폰할 때 PawnData, AbilitySystemComponent 초기화해줘야 함
		UAbilitySystemComponent* ASComponent = PNPlayerState->GetAbilitySystemComponent();
		PawnComponent->SetAbilitySystemComponent(CastChecked<UPNAbilitySystemComponent>(ASComponent));
		ASComponent->InitAbilityActorInfo(PNPlayerState, this);

		for (const UPNAbilitySet* AbilitySet : PawnComponent->GetPawnData()->AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveAbilityToAbilitySystem(ASComponent, this);
			}
		}
	}
}

void APNCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UPNPlayerComponent* PlayerComponent = FindComponentByClass<UPNPlayerComponent>())
	{
		PlayerComponent->InitializePlayerInput(PlayerInputComponent);
	}
}
