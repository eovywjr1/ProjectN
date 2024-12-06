// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PNGameplayAbility_AttackHitCheck.generated.h"

class APNTargetActor_HitCheckActor;
/**
 * 
 */
UCLASS()
class PROJECTN_API UPNGameplayAbility_AttackHitCheck : public UGameplayAbility
{
	GENERATED_BODY()

private:
	UPNGameplayAbility_AttackHitCheck();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;

	UFUNCTION()
	void OnTraceResultCallback(const TArray<TWeakObjectPtr<AActor>>& FoundActors);

private:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<APNTargetActor_HitCheckActor> TargetActorClass = nullptr;
};
