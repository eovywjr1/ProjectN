// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TargetActor/PNTargetActor_HitCheckActor_MultiSweep.h"

#include "Chaos/Private/Chaos/CollisionOneShotManifolds.h"

FGameplayAbilityTargetDataHandle APNTargetActor_HitCheckActor_MultiSweep::MakeTargetData() const
{
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_ActorArray* ActorArrayTargetData = new FGameplayAbilityTargetData_ActorArray();
	TargetDataHandle.Add(ActorArrayTargetData);

	switch (AttackHitBoxData.Shape)
	{
	case EHitBoxShape::Cone:
		{
			break;
		}
	case EHitBoxShape::Box:
		{
			GetHitBoxResult(TargetDataHandle);

			break;
		}
	}

	return TargetDataHandle;
}

void APNTargetActor_HitCheckActor_MultiSweep::GetHitBoxResult(FGameplayAbilityTargetDataHandle& OutTargetDataHandle) const
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(SourceActor);

	// Todo. 소켓 위치로 변경해야 함
	const FVector Start = SourceActor->GetActorLocation();
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(AttackHitBoxData.BoxExtent);

	GetWorld()->SweepMultiByChannel(HitResults, Start, Start, SourceActor->GetActorRotation().Quaternion(), ECC_Pawn, CollisionShape, QueryParams);

#ifdef ENABLE_DRAW_DEBUG
	DrawDebugBox(GetWorld(), Start, AttackHitBoxData.BoxExtent, SourceActor->GetActorRotation().Quaternion(), FColor::Green, false, 5.0f);
#endif

	HitResults.RemoveAll([this, Start, CollisionShape](const FHitResult& Hit)
	{
		return Cast<APawn>(Hit.GetActor()) == nullptr;
	});

	FGameplayAbilityTargetData_ActorArray* ActorArrayTargetData = static_cast<FGameplayAbilityTargetData_ActorArray*>(OutTargetDataHandle.Data[0].Get());

	for (const FHitResult& Hit : HitResults)
	{
		ActorArrayTargetData->TargetActorArray.AddUnique(Hit.GetActor());
	}
}
