// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TargetActor/PNTargetActor_HitCheckActor.h"

void APNTargetActor_HitCheckActor::StartTargeting(UGameplayAbility* Ability, const FHitBoxData& InAttackHitBoxData)
{
	Super::StartTargeting(Ability);

	AttackHitBoxData = InAttackHitBoxData;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
	                                       [this]() { this->CancelTargeting(); },
	                                       AttackHitBoxData.HitBoxDurationTime,
	                                       false);

	bFindTarget = true;
}

bool APNTargetActor_HitCheckActor::IsConfirmTargetingAllowed()
{
	return bFindTarget == false;
}
