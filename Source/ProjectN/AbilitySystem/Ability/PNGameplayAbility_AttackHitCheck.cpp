// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/PNGameplayAbility_AttackHitCheck.h"

#include "AbilitySystemComponent.h"
#include "PNGameplayTags.h"
#include "AbilitySystem/AttributeSet/PNWeaponAttributeSet.h"
#include "AbilitySystem/TargetActor/PNTargetActor_HitCheckActor.h"
#include "AbilitySystem/Task/PNAbilityTask_TraceToPawn.h"

UPNGameplayAbility_AttackHitCheck::UPNGameplayAbility_AttackHitCheck() 
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	FAbilityTriggerData AbilityTriggerData = FAbilityTriggerData();
	AbilityTriggerData.TriggerTag = FPNGameplayTags::Get().Ability_HitCheck;
	AbilityTriggers.Add(AbilityTriggerData);
}

void UPNGameplayAbility_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if(TargetActorClass)
	{
		UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();

		FGameplayTagContainer TagContainer;
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);

		FGameplayTag AttackTag;
		for (const FGameplayTag& GameplayTag : TagContainer)
		{
			if (GameplayTag.MatchesTag(FGameplayTag::RequestGameplayTag("Status.Attack")))
			{
				AttackTag = GameplayTag;
				break;
			}
		}

		const UPNWeaponAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UPNWeaponAttributeSet>();
		if (AttributeSet == nullptr)
		{
			return;
		}

		FHitBoxData AttackHitBoxData;
		AttributeSet->GetAttackHitBoxData(AttackTag, AttackHitBoxData);
	
		UPNAbilityTask_TraceToPawn* AttackTraceTask = UPNAbilityTask_TraceToPawn::CreateTask(this, TargetActorClass, AttackHitBoxData);
		AttackTraceTask->OnComplete.AddDynamic(this, &ThisClass::OnTraceResultCallback);
		AttackTraceTask->ReadyForActivation();
	}
}

void UPNGameplayAbility_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle) 
{

}
