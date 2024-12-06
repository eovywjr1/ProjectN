// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Task/PNAbilityTask_TraceToPawn.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/TargetActor/PNTargetActor_HitCheckActor.h"

void UPNAbilityTask_TraceToPawn::Activate()
{
	Super::Activate();

	bTickingTask = true;

	SpawnedTargetActor = Cast<APNTargetActor_HitCheckActor>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &ThisClass::OnTargetDataReadyCallback);
	}

	if (UAbilitySystemComponent* AbilitySystemComponentPtr = AbilitySystemComponent.Get())
	{
		const FTransform SpawnTransform = AbilitySystemComponentPtr->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTransform);

		AbilitySystemComponentPtr->SpawnedTargetActors.Push(SpawnedTargetActor);
		SpawnedTargetActor->SetAttackHitBoxData(HitBoxData);
		SpawnedTargetActor->StartTargeting(Ability);
	}

	SetWaitingOnAvatar();
}

void UPNAbilityTask_TraceToPawn::OnDestroy(bool bInOwnerFinished)
{
	if (IsValid(SpawnedTargetActor))
	{
		SpawnedTargetActor->Destroy();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UPNAbilityTask_TraceToPawn::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ElapsedTime += DeltaTime;
	if (HitBoxData.HitBoxDurationTime < ElapsedTime)
	{
		OnComplete.Broadcast(FoundActors);

		bTickingTask = false;
		EndTask();
	}

	SpawnedTargetActor->ConfirmTargetingAndContinue();
}

UPNAbilityTask_TraceToPawn* UPNAbilityTask_TraceToPawn::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<APNTargetActor_HitCheckActor> TargetActorClass, const FHitBoxData& InHitBoxData)
{
	UPNAbilityTask_TraceToPawn* NewTask = NewAbilityTask<UPNAbilityTask_TraceToPawn>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	NewTask->HitBoxData = InHitBoxData;
	return NewTask;
}

void UPNAbilityTask_TraceToPawn::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (TargetDataHandle.Data.IsEmpty())
	{
		return;
	}

	if (FGameplayAbilityTargetData_ActorArray* ActorArrayTargetData = static_cast<FGameplayAbilityTargetData_ActorArray*>(TargetDataHandle.Data[0].Get()))
	{
		for (const TWeakObjectPtr<AActor>& FoundActor : ActorArrayTargetData->GetActors())
		{
			FoundActors.AddUnique(FoundActor.Get());
		}
	}
}
