// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PNCharacterPlayer.h"

#include "PNActorGameData.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "Component/PNEnhancedInputComponent.h"
#include "Component/PNActorExtensionComponent.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PNPlayerState.h"

APNCharacterPlayer::APNCharacterPlayer()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	OverrideInputComponentClass = UPNEnhancedInputComponent::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void APNCharacterPlayer::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	CreateActorComponent(EActorType::Player);
}

void APNCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Controller)
	{
		if (!IsRun())
		{
			RunTargetRotationYaw = 0.0f;
		}

		FRotator TargetRotation = GetActorRotation();
		TargetRotation.Yaw = Controller->GetControlRotation().Yaw;

		if (RunTargetRotationYaw != 0.0f)
		{
			TargetRotation.Yaw += RunTargetRotationYaw;
		}

		const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, 10.0f);
		SetActorRotation(NewRotation);
	}
}

void APNCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeAbilitySystemComponent();
}

void APNCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystemComponent();
}

void APNCharacterPlayer::InitializeAbilitySystemComponent()
{
	APNPlayerState* PlayerStateCast = GetPlayerState<APNPlayerState>();
	check(PlayerStateCast);

	UAbilitySystemComponent* AbilitySystemComponent = PlayerStateCast->GetAbilitySystemComponent();
	ActorExtensionComponent->InitializeAbilitySystem(Cast<UPNAbilitySystemComponent>(AbilitySystemComponent), PlayerStateCast);
}

UPNActorGameData* APNCharacterPlayer::GetActorGameData() const
{
	const FName ActorGameDataFileName = TEXT("PlayerGameData");
	const UAssetManager& AssetManager = UAssetManager::Get();
	FSoftObjectPtr AssetPtr(AssetManager.GetPrimaryAssetPath(FPrimaryAssetId(FName(TEXT("ActorGameData")), ActorGameDataFileName)));
	if (AssetPtr.IsPending())
	{
		AssetPtr.LoadSynchronous();
	}

	UPNActorGameData* ActorGameData = Cast<UPNActorGameData>(AssetPtr.Get());

	return ActorGameData;
}

void APNCharacterPlayer::MoveByInput(const FVector2D MovementVector)
{
	check(Controller);

	if (!IsMovable())
	{
		return;
	}

	if (IsRun() && MovementVector.Y >= 0.0f && FMath::Abs(MovementVector.X) > 0.0f)
	{
		RunTargetRotationYaw = FMath::RadiansToDegrees(FMath::Atan2(MovementVector.X, MovementVector.Y));
	}
	else
	{
		RunTargetRotationYaw = 0.0f;
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
