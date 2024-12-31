// Fill out your copyright notice in the Description page of Project Settings.


#include "PNTargetMarkerUserWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

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
				if (PlayerController->ProjectWorldLocationToScreen(LockOnTargetObject->GetActorLocation(), ScreenPosition))
				{
					if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
					{
						FVector2D ViewportSize;
						GEngine->GameViewport->GetViewportSize(ViewportSize);

						FVector2D PanelSize;
						if (UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(GetParent()))
						{
							PanelSize = CanvasPanel->GetPaintSpaceGeometry().GetLocalSize();
						}

						const float PanelPerViewportRatioX = PanelSize.X / ViewportSize.X;
						const float PanelPerViewportRatioY = PanelSize.Y / ViewportSize.Y;
						const FVector2D PanelPosition(ScreenPosition.X * PanelPerViewportRatioX, ScreenPosition.Y * PanelPerViewportRatioY);

						CanvasSlot->SetPosition(PanelPosition);
					}
				}
			}
		}
	}
}
