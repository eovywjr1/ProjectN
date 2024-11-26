// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/PNGameplayAbility_Attack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UPNGameplayAbility_Attack::UPNGameplayAbility_Attack() 
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPNGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* AvatarActor = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	AvatarActor->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), ComboActionMontage, 1.0f, GetNextSectionName());
	PlayAttackTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();
}

void UPNGameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* AvatarActor = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	AvatarActor->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPNGameplayAbility_Attack::OnCompleteCallback()
{
	bool bReplicateEndAbility = false;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPNGameplayAbility_Attack::OnInterruptedCallback()
{
	bool bReplicateEndAbility = false;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}