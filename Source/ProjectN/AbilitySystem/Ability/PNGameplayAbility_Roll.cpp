// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/PNGameplayAbility_Roll.h"

#include "PNGameplayTags.h"
#include "PNLogChannels.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/PNPlayerComponent.h"

UGameplayAbilityTask_Roll* UGameplayAbilityTask_Roll::CreateRollProxy(UGameplayAbility* OwningAbility, FName TaskInstanceName, float InDuration, float InDistance, FVector InDirection)
{
	check(OwningAbility);

	UGameplayAbilityTask_Roll* MyObj = NewAbilityTask<UGameplayAbilityTask_Roll>(OwningAbility, TaskInstanceName);
	MyObj->Duration = InDuration;
	MyObj->Distance = InDistance;
	MyObj->Direction = InDirection;

	return MyObj;
}

void UGameplayAbilityTask_Roll::Activate()
{
	Super::Activate();

	bTickingTask = true;
}

void UGameplayAbilityTask_Roll::TickTask(float DeltaTime)
{
	AActor* Avatar = GetAvatarActor();
	if (Avatar == nullptr)
	{
		return;
	}

	const FVector NewLocation = Avatar->GetActorLocation() + Direction * (Distance / Duration) * DeltaTime;
	Avatar->SetActorLocation(NewLocation);
}

UPNGameplayAbility_Roll::UPNGameplayAbility_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationOwnedTags.AddTag(FPNGameplayTags::FindTagByString("Status.Roll"));
}

void UPNGameplayAbility_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APawn* AvatarActor = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (AvatarActor == nullptr)
	{
		return;
	}

	UPNPlayerComponent* PlayerComponent = AvatarActor->FindComponentByClass<UPNPlayerComponent>();
	if (PlayerComponent == nullptr)
	{
		return;
	}

	PlayerComponent->EnableInput(false);

	if (RollActionMontage)
	{
		UAbilityTask_PlayMontageAndWait* RollMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Roll"), RollActionMontage);
		RollMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleteCallback);
		RollMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterruptCallback);
		RollMontageTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogPN, Error, TEXT("ActionMontage Not Found in Roll Ability"));
	}

	FVector Direction = AvatarActor->GetLastMovementInputVector();
	if (Direction.IsNearlyZero())
	{
		Direction = AvatarActor->GetActorForwardVector();
	}

	UGameplayAbilityTask_Roll* RollTask = UGameplayAbilityTask_Roll::CreateRollProxy(this, TEXT("Roll"), RollDuration, RollDistance, Direction);
	RollTask->ReadyForActivation();
}

void UPNGameplayAbility_Roll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor == nullptr)
	{
		return;
	}

	UPNPlayerComponent* PlayerComponent = AvatarActor->FindComponentByClass<UPNPlayerComponent>();
	if (PlayerComponent == nullptr)
	{
		return;
	}

	PlayerComponent->EnableInput(true);
}

void UPNGameplayAbility_Roll::OnCompleteCallback()
{
	bool bReplicateEndAbility = false;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPNGameplayAbility_Roll::OnInterruptCallback()
{
	bool bReplicateEndAbility = false;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
