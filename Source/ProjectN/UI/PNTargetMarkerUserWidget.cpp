// Fill out your copyright notice in the Description page of Project Settings.


#include "PNTargetMarkerUserWidget.h"

void UPNTargetMarkerUserWidget::SetTarget(FObjectKey InLockOnTargetObjectKey)
{
	LockOnTargetObjectKey = InLockOnTargetObjectKey;

	SetVisibility(ESlateVisibility::Visible);
}

void UPNTargetMarkerUserWidget::DeactivateMarker()
{
	LockOnTargetObjectKey = FObjectKey();

	SetVisibility(ESlateVisibility::Hidden);
}

void UPNTargetMarkerUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);
}

void UPNTargetMarkerUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GetVisibility() == ESlateVisibility::Visible)
	{
		AActor* LockOnTargetObject = Cast<AActor>(LockOnTargetObjectKey.ResolveObjectPtr());
		if (IsValid(LockOnTargetObject))
		{
			if (APlayerController* PlayerController = GetOwningPlayer())
			{
				FVector2D ScreenPosition;
				const FVector TargetLocation = LockOnTargetObject->GetActorLocation();
				if (PlayerController->ProjectWorldLocationToScreen(TargetLocation, ScreenPosition))
				{
					SetPositionInViewport(ScreenPosition);
				}
			}
		}
	}
}
