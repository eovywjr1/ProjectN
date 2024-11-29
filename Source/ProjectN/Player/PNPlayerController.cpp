// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PNPlayerController.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

APNPlayerController::APNPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  LockOnTargetVisibleAreaRate(30)
{}

void APNPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(LockOnTargetActor))
	{
		const FVector TargetLocation = LockOnTargetActor->GetActorLocation();
		const FRotator NewRotation = (TargetLocation - PlayerCameraManager->GetCameraLocation()).Rotation();
		SetControlRotation(NewRotation);

#ifdef ENABLE_DRAW_DEBUG
		DrawDebugPoint(GetWorld(), TargetLocation, 40.0f, FColor::White, false, -1.0f);
#endif
	}
}

bool APNPlayerController::CanCameraInputControl() const
{
	if (IsValid(LockOnTargetActor))
	{
		return false;
	}

	return true;
}

void APNPlayerController::SetLockOnTargetActor()
{
	APawn* OwnerPawn = GetPawn();
	if (OwnerPawn == nullptr)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	const FVector OwnerLocation = OwnerPawn->GetActorLocation();
	const UWorld* World = GetWorld();
	UKismetSystemLibrary::SphereOverlapActors(World, OwnerLocation, SearchLockOnTargetRadius, TArray<TEnumAsByte<EObjectTypeQuery>>(), APawn::StaticClass(), ActorsToIgnore, OverlappingActors);

#ifdef ENABLE_DRAW_DEBUG
	DrawDebugSphere(World, OwnerLocation, SearchLockOnTargetRadius, 32, FColor::Red, true, 1.0f);
#endif

	AActor* NearestTarget = nullptr;
	float NearestDistance = FLT_MAX;

	for (AActor* Actor : OverlappingActors)
	{
		if (CanLockOnTargetActor(Actor) == false)
		{
			continue;
		}

		const float Distance = FVector::Distance(OwnerLocation, Actor->GetActorLocation());

		if (NearestTarget == nullptr)
		{
			NearestTarget = Actor;
			NearestDistance = Distance;
		}
		else if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestTarget = Actor;
		}
	}

	LockOnTargetActor = NearestTarget;

	if(LockOnTargetActor && CheckLockOnTimerHandle.IsValid() == false)
	{
		GetWorld()->GetTimerManager().SetTimer(CheckLockOnTimerHandle, this, &ThisClass::CheckLockOnTimerCallback, CheckLockOnTimerPeriod, false, CheckLockOnTimerPeriod);
	}
}

bool APNPlayerController::CanLockOnTargetActor(AActor* TargetActor) const
{
	if (IsValid(TargetActor) == false)
	{
		return false;
	}

	// Todo. 공격 판정 / 죽음 구현 후 체크해야 함
	// if (TargetActor->Dead())
	// {
	// 	return false;
	// }

	APawn* OwnerPawn = GetPawn();
	if (OwnerPawn == nullptr || TargetActor == OwnerPawn)
	{
		return false;
	}

	if (PlayerCameraManager == nullptr)
	{
		return false;
	}

	// 일단 캡슐 컴포넌트만 가능, 추후 변경해도 됨
	UCapsuleComponent* CapsuleComponent = TargetActor->FindComponentByClass<UCapsuleComponent>();
	if (CapsuleComponent == nullptr)
	{
		return false;
	}

	const FVector OwnerPawnLocation = OwnerPawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	if (SearchLockOnTargetRadius < FVector::Distance(OwnerPawnLocation, TargetLocation))
	{
		return false;
	}

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FTransform WorldTransform = CapsuleComponent->GetComponentTransform();
	const float ScaledRadius = CapsuleComponent->GetScaledCapsuleRadius();
	const float ScaledHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();

	constexpr uint8 GridDivisionCount = 10;

	const float BoxHeight = ScaledHalfHeight * 2.0f / GridDivisionCount;
	const float BoxWidth = (ScaledRadius * 2) / GridDivisionCount;
	const float BoxDepth = (ScaledRadius * 2) / GridDivisionCount;

	uint8 TotalBoxCount = 0;
	uint8 VisibleBoxCount = 0;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn);

	for (uint8 HeightIndex = 0; HeightIndex < GridDivisionCount; HeightIndex++)
	{
		for (uint8 WidthIndex = 0; WidthIndex < GridDivisionCount; WidthIndex++)
		{
			for (uint8 DepthIndex = 0; DepthIndex < GridDivisionCount; DepthIndex++)
			{
				FVector BoxCenter = WorldTransform.GetLocation();
				BoxCenter.X += (-ScaledRadius + (BoxWidth / 2) + (BoxWidth * WidthIndex));
				BoxCenter.Y += (-ScaledRadius + (BoxDepth / 2) + (BoxDepth * DepthIndex));
				BoxCenter.Z += (-ScaledHalfHeight + (BoxHeight / 2) + (BoxHeight * HeightIndex));

				const FVector LocalPoint = WorldTransform.InverseTransformPosition(BoxCenter);
				if (ScaledHalfHeight < FMath::Abs(LocalPoint.Z) || ScaledRadius < FMath::Abs(LocalPoint.X))
				{
					continue;
				}

				const float RadialDistance = FMath::Sqrt(LocalPoint.X * LocalPoint.X + LocalPoint.Y * LocalPoint.Y);
				if (RadialDistance > ScaledRadius)
				{
					continue;
				}

				++TotalBoxCount;

				FVector2D ScreenPosition;
				if (ProjectWorldLocationToScreen(BoxCenter, ScreenPosition) == false)
				{
					continue;
				}

				int32 ViewportSizeX, ViewportSizeY;
				GetViewportSize(ViewportSizeX, ViewportSizeY);

				const bool bIsOnScreen = ScreenPosition.X >= 0 && ScreenPosition.X <= ViewportSizeX && ScreenPosition.Y >= 0 && ScreenPosition.Y <= ViewportSizeY;
				if (bIsOnScreen == false)
				{
					continue;
				}

				FHitResult HitResult;
				const FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(BoxWidth / 2, 1.0f, BoxHeight / 2));
				const FQuat BoxRotation = WorldTransform.GetRotation();
				const bool bHit = GetWorld()->SweepSingleByChannel(HitResult, BoxCenter, CameraLocation, BoxRotation, ECC_Visibility, BoxShape, QueryParams);
				if (bHit == false)
				{
					++VisibleBoxCount;
				}

				// #ifdef ENABLE_DRAW_DEBUG
				// 					const FColor BoxColor = bHit == false ? FColor::Green : FColor::Red;
				// 					DrawDebugBox(World, BoxCenter, FVector(BoxWidth / 2, 1.0f, BoxHeight / 2), BoxRotation, BoxColor, false, 5.0f);
				// #endif
			}
		}
	}

	if (LockOnTargetVisibleAreaRate > FPNPercent::FromFraction(VisibleBoxCount, TotalBoxCount))
	{
		return false;
	}

	return true;
}

void APNPlayerController::CheckLockOnTimerCallback()
{
	CheckLockOnTimerHandle.Invalidate();
	
	if (CanLockOnTargetActor(LockOnTargetActor))
	{
		GetWorld()->GetTimerManager().SetTimer(CheckLockOnTimerHandle, this, &ThisClass::CheckLockOnTimerCallback, CheckLockOnTimerPeriod, false, CheckLockOnTimerPeriod);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ClearLockOnTargetTimerHandle, [this]() { LockOnTargetActor = nullptr; }, ClearLockOnTimerPeriod, false, ClearLockOnTimerPeriod);
	}
}
