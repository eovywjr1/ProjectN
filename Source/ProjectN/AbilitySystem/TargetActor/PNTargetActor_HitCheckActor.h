// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystem/AttributeSet/PNWeaponAttributeSet.h"
#include "PNTargetActor_HitCheckActor.generated.h"

class UPNWeaponAttributeSet;

/**
 * 
 */
UCLASS()
class PROJECTN_API APNTargetActor_HitCheckActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	virtual void StartTargeting(UGameplayAbility* Ability, const FHitBoxData& InAttackHitBoxData);
	
protected:
	virtual bool IsConfirmTargetingAllowed() override;
	
protected:
	bool bFindTarget = false;
	FHitBoxData AttackHitBoxData;
};
