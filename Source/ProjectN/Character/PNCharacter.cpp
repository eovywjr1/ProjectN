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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	/////////////////////////////////////////////////////////////////////////////////////////
	
	OverrideInputComponentClass = UPNEnhancedInputComponent::StaticClass();
	
	PawnComponent = CreateDefaultSubobject<UPNPawnComponent>(TEXT("PNPawnComponent"));
}

void APNCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// if (APNPlayerState* CharacterPlayerState = GetPlayerState<APNPlayerState>())
	// {
	// 	AbilitySystemComponent = CharacterPlayerState->GetAbilitySystemComponent();
	// 	AbilitySystemComponent->InitAbilityActorInfo(CharacterPlayerState, this);
	//
	// 	int32 InputID = 0;
	//
	// 	// for (const TSubclassOf<UGameplayAbility>& Ability : Abilities)
	// 	// {
	// 	// 	FGameplayAbilitySpec AbilitySpec(Ability);
	// 	// 	AbilitySpec.InputID = InputID;
	// 	// 	++InputID;
	// 	//
	// 	// 	AbilitySystemComponent->GiveAbility(AbilitySpec);
	// 	// }
	// }
}

//////////////////////////////////////////////////////////////////////////
// Input

void APNCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// 이 시점에서 InputComponent가 유효함
	if (UPNPlayerComponent* PlayerComponent = FindComponentByClass<UPNPlayerComponent>())
	{
		PlayerComponent->InitializePlayerInput(PlayerInputComponent);
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APNCharacter::Look);
	}
}

void APNCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
