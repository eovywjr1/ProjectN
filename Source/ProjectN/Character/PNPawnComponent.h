// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"

#include "PNPawnComponent.generated.h"

/**
 * 
 */

class UAbilitySystemComponent;
class UPNPawnData;
 
UCLASS()
class PROJECTN_API UPNPawnComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UPNPawnComponent(const FObjectInitializer& ObjectInitializer);
	
	const UPNPawnData* GetPawnData() const { return PawnData; }
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	void SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent) { AbilitySystemComponent = InAbilitySystemComponent; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Pawn")
	TObjectPtr<const UPNPawnData> PawnData;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
