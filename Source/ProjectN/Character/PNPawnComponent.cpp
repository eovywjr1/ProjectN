// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PNPawnComponent.h"

#include "AbilitySystemComponent.h"
#include "PNPlayerComponent.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"

UPNPawnComponent::UPNPawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}

void UPNPawnComponent::BeginPlay()
{
	Super::BeginPlay();

	// Todo. 임시 테스트용도, 추후 액터 타입에 따라 추가해야 함
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), GetOwner());
	}
}
