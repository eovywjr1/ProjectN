// Fill out your copyright notice in the Description page of Project Settings.


#include "PNPawnComponent.h"

#include "PNPlayerComponent.h"

UPNPawnComponent::UPNPawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}