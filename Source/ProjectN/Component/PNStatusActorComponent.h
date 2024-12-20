// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "PNStatusActorComponent.generated.h"

class UPNPawnAttributeSet;

enum class EStatusType : uint8;
enum class EEquipSlotType : uint8;

struct FEquipmentDataTable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTN_API UPNStatusActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void ApplyStatusFromEquipment(const FEquipmentDataTable* EquipmentDataTable);
	void UnApplyStatusFromEquipment(const EEquipSlotType EquipSlot);

private:
	virtual void BeginPlay() override final;
	
	const FGameplayAttribute GetEquipmentStatusAttribute(const EStatusType StatusType) const;
	
private:
	UPROPERTY()
	TMap<EEquipSlotType, FActiveGameplayEffectHandle> ActiveEquipStatusEffectHandles;
};
