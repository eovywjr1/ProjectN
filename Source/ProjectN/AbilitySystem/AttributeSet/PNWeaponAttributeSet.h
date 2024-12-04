// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PNCommonModule.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AttributeSet/PNAttributeSet.h"
#include "PNWeaponAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTN_API UPNWeaponAttributeSet : public UPNAttributeSet
{
	GENERATED_BODY()
	
public:
	bool GetAttackHitBoxData(FGameplayTag AbilityTag, FHitBoxData& OutAttackHitBoxData) const;

private:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TMap<FGameplayTag, FHitBoxData> AttackHitBoxes;
};
