// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TargetActor/PNTargetActor_HitCheckActor_MultiSweep.h"

void APNTargetActor_HitCheckActor_MultiSweep::ConfirmTargetingAndContinue()
{
	Super::ConfirmTargetingAndContinue();

	TArray<AActor*> HitActors;
	
	switch (AttackHitBoxData.Shape)
	{
	case EHitBoxShape::Cone:
		{
			ConeTrace(HitActors);
		
			break;
		}
	case EHitBoxShape::Box:
		{
			break;
		}
	}
}

void APNTargetActor_HitCheckActor_MultiSweep::ConeTrace(TArray<AActor*>& OutHitActors) const
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(SourceActor);

	const FVector SourceActorLocation = SourceActor->GetActorLocation();
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackHitBoxData.Range);
	GetWorld()->SweepMultiByChannel(
		HitResults,
		SourceActorLocation,
		SourceActorLocation,
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		QueryParams
	);

	const FVector Forward = SourceActor->GetActorForwardVector();
	const float HalfAngle = FMath::DegreesToRadians(AttackHitBoxData.Angle * 0.5f);

	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			FVector DirectionToTarget = (HitActor->GetActorLocation() - SourceActorLocation).GetSafeNormal();
			float DotProduct = FVector::DotProduct(Forward, DirectionToTarget);
			float CurrentAngle = FMath::Acos(DotProduct);

			if (CurrentAngle <= HalfAngle)
			{
				OutHitActors.AddUnique(HitActor);
			}
		}
	}
}
