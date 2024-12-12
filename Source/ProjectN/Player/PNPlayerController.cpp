// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PNPlayerController.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

constexpr uint8 GridDivisionCount = 10;
constexpr uint32 TotalGridPoints = GridDivisionCount * GridDivisionCount * GridDivisionCount;

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

void APNPlayerController::RotationByInput(const FVector2D LookAxisVector) 
{
	if (CanCameraInputControl() == false)
	{
		return;
	}

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
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
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), OwnerLocation, SearchLockOnTargetRadius, TArray<TEnumAsByte<EObjectTypeQuery>>(), APawn::StaticClass(), ActorsToIgnore, OverlappingActors);

#ifdef ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), OwnerLocation, SearchLockOnTargetRadius, 32, FColor::Red, true, 1.0f);
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

	if (LockOnTargetActor && CheckLockOnTimerHandle.IsValid() == false)
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

	// 일단 캡슐 컴포넌트만 가능, 추후 변경 가능
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
	const FTransform ComponentTransform = CapsuleComponent->GetComponentTransform();

	const float ScaledRadius = CapsuleComponent->GetScaledCapsuleRadius();
	const float ScaledHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();

	const float BoxHeight = ScaledHalfHeight * 2.0f / GridDivisionCount;
	const float BoxWidth = (ScaledRadius * 2) / GridDivisionCount;
	const float BoxDepth = (ScaledRadius * 2) / GridDivisionCount;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn);

	const float FOVRadians = FMath::DegreesToRadians(PlayerCameraManager->GetFOVAngle());
	const float CosHalfFOV = FMath::Cos(FOVRadians * 0.5f);

	TArray<FVector> BoxCenters;
	FCriticalSection BoxCenterCriticalSection;
	BoxCenters.Reserve(TotalGridPoints);

	FThreadSafeCounter TotalBoxCounter(0);

	ParallelFor(TotalGridPoints,
	            [this, ScaledHalfHeight, ScaledRadius, BoxHeight, BoxWidth, BoxDepth, ComponentTransform, CameraLocation, CosHalfFOV, &BoxCenters, &BoxCenterCriticalSection, &TotalBoxCounter](int32 Index)
	            {
		            const int32 HeightIndex = Index / (GridDivisionCount * GridDivisionCount);
		            const int32 WidthIndex = (Index / GridDivisionCount) % GridDivisionCount;
		            const int32 DepthIndex = Index % GridDivisionCount;

		            FVector BoxCenter = ComponentTransform.GetLocation();
		            BoxCenter.X += (-ScaledRadius + (BoxWidth / 2) + (BoxWidth * WidthIndex));
		            BoxCenter.Y += (-ScaledRadius + (BoxDepth / 2) + (BoxDepth * DepthIndex));
		            BoxCenter.Z += (-ScaledHalfHeight + (BoxHeight / 2) + (BoxHeight * HeightIndex));

		            const FVector LocalBoxCenter = ComponentTransform.InverseTransformPosition(BoxCenter);
		            if (ScaledHalfHeight < FMath::Abs(LocalBoxCenter.Z) || ScaledRadius < FMath::Abs(LocalBoxCenter.X))
		            {
			            return;
		            }

		            const float RadialDistance = FMath::Sqrt(LocalBoxCenter.X * LocalBoxCenter.X + LocalBoxCenter.Y * LocalBoxCenter.Y);
		            if (RadialDistance > ScaledRadius)
		            {
			            return;
		            }

		            TotalBoxCounter.Increment();

		            const FVector DirectionCameraToBox = (BoxCenter - CameraLocation).GetSafeNormal();
		            const float CosAngleCameraToBox = FVector::DotProduct(PlayerCameraManager->GetActorForwardVector(), DirectionCameraToBox);
		            if (CosAngleCameraToBox < CosHalfFOV)
		            {
			            return;
		            }

		            {
			            FScopeLock Lock(&BoxCenterCriticalSection);
			            BoxCenters.Add(BoxCenter);
		            }
	            });

	uint8 VisibleBoxCount = 0;

	for (const FVector& BoxCenter : BoxCenters)
	{
		FHitResult HitResult;
		const FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(BoxWidth / 2, 1.0f, BoxHeight / 2));
		const FQuat BoxRotation = ComponentTransform.GetRotation();
		const bool bHit = GetWorld()->SweepSingleByChannel(HitResult, BoxCenter, CameraLocation, BoxRotation, ECC_Visibility, BoxShape, QueryParams);
		if (bHit == false)
		{
			++VisibleBoxCount;
		}

		// #ifdef ENABLE_DRAW_DEBUG
		// 		const FColor BoxColor = bHit == false ? FColor::Green : FColor::Red;
		// 		DrawDebugBox(GetWorld(), BoxCenter, FVector(BoxWidth / 2, 1.0f, BoxHeight / 2), BoxRotation, BoxColor, false, 5.0f);
		// #endif
	}

	return LockOnTargetVisibleAreaRate <= FPNPercent::FromFraction(VisibleBoxCount, TotalBoxCounter.GetValue());
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
