// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PNCharacterPlayer.h"

#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "Component/PNEnhancedInputComponent.h"
#include "Component/PNPlayerInputComponent.h"
#include "Component/PNEquipmentComponent.h"
#include "Component/PNInventoryComponent.h"
#include "Component/PNPawnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PNPlayerState.h"

APNCharacterPlayer::APNCharacterPlayer()
{
	bUseControllerRotationYaw = true;

	OverrideInputComponentClass = UPNEnhancedInputComponent::StaticClass();

	CreateDefaultSubobject<UPNEquipmentComponent>(TEXT("EquipmentComponent"));
	PawnComponent->ActorType = EActorType::Player;
}

void APNCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitialComponents();
}

void APNCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitialComponents();
}

void APNCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PNPlayerInputComponent)
	{
		PNPlayerInputComponent->InitializePlayerInput(InputComponent);
	}
}

void APNCharacterPlayer::InitialComponents()
{
	APNPlayerState* PlayerStateCast = GetPlayerState<APNPlayerState>();
	check(PlayerStateCast);

	UPNAbilitySystemComponent* AbilitySystemComponent = PlayerStateCast->GetAbilitySystemComponent();
	PawnComponent->InitializeAbilitySystem(AbilitySystemComponent, PlayerStateCast);

	if (IsLocallyControlled())
	{
		PNPlayerInputComponent = NewObject<UPNPlayerInputComponent>(this, TEXT("PlayerInputComponent"));
		PNPlayerInputComponent->RegisterComponent();
		
		// PlayerState 리플리케이션이 늦을 경우 InputComponent가 먼저 생생하는 상황 대비하여 초기화
		// 보통은 PlayerInputComponent가 먼저 생성되므로 InputComponent가 nullptr
		PNPlayerInputComponent->InitializePlayerInput(InputComponent);

		UPNInventoryComponent* InventoryComponent = NewObject<UPNInventoryComponent>(this, TEXT("InventoryComponent"));
		InventoryComponent->RegisterComponent();
	}
}

void APNCharacterPlayer::MoveByInput(const FVector2D MovementVector)
{
	check(Controller);

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	if (IsRun() && MovementVector.Y >= 0.0f && FMath::Abs(MovementVector.X) > 0.0f)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APNCharacterPlayer::SetDead()
{
	Super::SetDead();

	DisableInput(Cast<APlayerController>(GetController()));
}
