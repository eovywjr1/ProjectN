﻿// Copyright Epic Games, Inc. All Rights Reserved.

#include "PNCharacter.h"

#include "PNGameplayTags.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/PNPawnAttributeSet.h"
#include "Component/PNCharacterMovementComponent.h"
#include "Component/PNActorExtensionComponent.h"
#include "Component/PNSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////

void APNCharacter::SetMaxWalkSpeed(const float InMaxSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = InMaxSpeed;

	if (!HasAuthority())
	{
		ServerSetMaxWalkSpeed(InMaxSpeed);
	}
}

float APNCharacter::GetMaxWalkSpeed() const
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void APNCharacter::ServerSetMaxWalkSpeed_Implementation(const float InMaxSpeed)
{
	SetMaxWalkSpeed(InMaxSpeed);
}

void APNCharacter::OnInitializedStatus() const
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = AbilitySystemComponent->GetSet<UPNPawnAttributeSet>()->GetWalkSpeed();
}

void APNCharacter::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetActorEnableCollision(false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	if (DeadMontage)
	{
		AnimInstance->Montage_Play(DeadMontage, 1.0f);
	}
}

void APNCharacter::SetMovable(bool bInMovable)
{
	if (bMovable == bInMovable)
	{
		return;
	}
	
	bMovable = bInMovable;
	
	if(IsClientActor(this))
	{
		ServerSetMovable(bInMovable);
	}
}

APNCharacter::APNCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPNCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
	, bMovable(true)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->SetIsReplicated(true);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void APNCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ActorExtensionComponent = FindComponentByClass<UPNActorExtensionComponent>();
	check(ActorExtensionComponent);
}

void APNCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bMovable);
}

void APNCharacter::ServerSetMovable_Implementation(bool InIsMovable)
{
	bMovable = InIsMovable;
}

UAbilitySystemComponent* APNCharacter::GetAbilitySystemComponent() const
{
	if (ActorExtensionComponent == nullptr)
	{
		ActorExtensionComponent = FindComponentByClass<UPNActorExtensionComponent>();
		check(ActorExtensionComponent);
	}

	return ActorExtensionComponent->GetAbilitySystemComponent();
}

bool APNCharacter::IsPlayer() const
{
	if (Controller == nullptr)
	{
		return false;
	}

	return Controller->IsPlayerController();
}

bool APNCharacter::IsRun() const
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (AbilitySystemComponent == nullptr)
	{
		return false;
	}

	return AbilitySystemComponent->HasMatchingGameplayTag(FPNGameplayTags::Get().Action_Run);
}
