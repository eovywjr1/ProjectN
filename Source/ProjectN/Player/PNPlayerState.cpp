// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PNPlayerState.h"

#include "AbilitySystemComponent.h"

APNPlayerState::APNPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}
