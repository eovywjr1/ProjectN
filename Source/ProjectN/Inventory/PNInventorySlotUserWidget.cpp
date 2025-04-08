// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PNInventorySlotUserWidget.h"

#include "DataTable/ItemDataTable.h"
#include "Subsystem/PNGameDataSubsystem.h"

void UPNInventorySlotUserWidget::UpdateSlot()
{
	const FItemDataTable* ItemDataTable = UPNGameDataSubsystem::Get(GetWorld())->GetData<FItemDataTable>(ItemKey);

	if (ItemDataTable == nullptr)
	{
		ensure(false);
		return;
	}

	// ItemDataTable->GetItemImage()
	// Image update
}
