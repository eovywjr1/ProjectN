// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PNPlayerState.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"

UPNAbilitySystemComponent* APNPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

APNPlayerState::APNPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UPNAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}
