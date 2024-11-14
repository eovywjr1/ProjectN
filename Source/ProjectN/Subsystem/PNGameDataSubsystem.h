// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PNGameDataSubsystem.generated.h"

struct FTableRowBase;

/**
 * 
 */
UCLASS()
class PROJECTN_API UPNGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;

	template <typename T>
	const T* GetData(const FName& Key)
	{
		static_assert(TIsDerivedFrom<T, FTableRowBase>::Value, "T must inherit from FTableRowBase");

		if (const TMap<FName, TObjectPtr<const FTableRowBase>>* TypeMap = DataMap.Find(T::StaticStruct()))
		{
			if (const TObjectPtr<const FTableRowBase>* FoundData = TypeMap->Find(Key))
			{
				return static_cast<const T*>(FoundData->Get());
			}
		}
		
		ensureMsgf(false, TEXT("%s 의 Row(%s)가 없습니다."), *T::StaticStruct()->GetFName().ToString(), *Key.ToString());
		
		return nullptr;
	}

private:
	void LoadDataTable();

	TMap<TObjectPtr<const UScriptStruct>, TMap<FName, TObjectPtr<const FTableRowBase>>> DataMap;
};
