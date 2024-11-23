// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "PNPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTN_API APNPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APNPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override final { return AbilitySystemComponent; }
	
private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
