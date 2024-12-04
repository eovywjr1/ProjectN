// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSet/PNWeaponAttributeSet.h"

#include "GameplayTagContainer.h"

bool UPNWeaponAttributeSet::GetAttackHitBoxData(FGameplayTag AbilityTag, FHitBoxData& OutAttackHitBoxData) const
{
	if (const FHitBoxData* Data = AttackHitBoxes.Find(AbilityTag))
	{
		OutAttackHitBoxData = *Data;
		return true;
	}

	return false;
}
