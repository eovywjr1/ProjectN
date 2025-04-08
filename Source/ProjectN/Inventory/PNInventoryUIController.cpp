// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PNInventoryUIController.h"

#include "PNInventoryComponent.h"
#include "PNInventorySlotUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PNHUD.h"

void UPNInventoryUIController::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APNHUD* HUD = Cast<APNHUD>(PlayerController->GetHUD());

	HUD->OnUpdateInventoryDelegate.AddDynamic(this, &ThisClass::OnUpdateInventory);
}

void UPNInventoryUIController::OnUpdateInventory()
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		UPNInventoryComponent* Inventory = PlayerPawn->FindComponentByClass<UPNInventoryComponent>();
		const TArray<FPNInventorySlot>& InventorySlots = Inventory->GetSlots();
		TSet<FName> CurrentItemKeys;

		for (const FPNInventorySlot& InventorySlot : InventorySlots)
		{
			const FName ItemKey = InventorySlot.GetItemKey();
			check(ItemKey.IsValid() && ItemKey != NAME_None);
			
			CurrentItemKeys.Add(ItemKey);
			
			if (TObjectPtr<UPNUserWidget>* Widget = InventorySlotWidgets.Find(InventorySlot.GetItemKey()))
			{
				UPNInventorySlotUserWidget* InventorySlotWidget = Cast<UPNInventorySlotUserWidget>(*Widget);
				check(InventorySlotWidget);

				InventorySlotWidget->UpdateSlot();
			}
			else
			{
				UPNUserWidget* NewSlotUserWidget = AddInventorySlotUserWidget();
				InventorySlotWidgets.Add(InventorySlot.GetItemKey(), NewSlotUserWidget);
			}
		}

		TArray<FName> ItemKeysToRemove;
		
		for (const TPair<FName, TObjectPtr<UPNUserWidget>>& InventorySlotWidgetPair : InventorySlotWidgets)
		{
			if (!CurrentItemKeys.Contains(InventorySlotWidgetPair.Key))
			{
				if (InventorySlotWidgetPair.Value)
				{
					InventorySlotWidgetPair.Value->RemoveFromParent();
				}
                
				ItemKeysToRemove.Add(InventorySlotWidgetPair.Key);
			}
		}

		for (const FName& KeyToRemove : ItemKeysToRemove)
		{
			InventorySlotWidgets.Remove(KeyToRemove);
		}
	}
}
