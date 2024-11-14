// Fill out your copyright notice in the Description page of Project Settings.


#include "PNGameDataSubsystem.h"

#include "Engine/DataTable.h"
#include "Engine/ObjectLibrary.h"

void UPNGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	LoadDataTable();
}

void UPNGameDataSubsystem::LoadDataTable()
{
	const FString DataTablePath = TEXT("/Game/ProjectN/DataTable");
	
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UDataTable::StaticClass(), false, GIsEditor);
	ObjectLibrary->AddToRoot();
	ObjectLibrary->LoadAssetDataFromPath(DataTablePath);
	ObjectLibrary->LoadAssetsFromAssetData();

	TArray<FAssetData> DataTables;
	ObjectLibrary->GetAssetDataList(DataTables);

	for (const FAssetData& AssetData : DataTables)
	{
		if (UDataTable* DataTable = Cast<UDataTable>(AssetData.GetAsset()))
		{
			const UScriptStruct* DataTableStruct = DataTable->GetRowStruct();
			TMap<FName, TObjectPtr<const FTableRowBase>>& TypeMap = DataMap.FindOrAdd(DataTableStruct);
			
			for (const FName& RowName : DataTable->GetRowNames())
			{
				if (const FTableRowBase* Data = DataTable->FindRow<FTableRowBase>(RowName, TEXT("")))
				{
					TypeMap.Add(RowName, Data);
				}
			}
		}
	}
}
