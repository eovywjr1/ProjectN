// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PNPawnData.generated.h"

class UPNInputConfig;

/**
 * 
 */
UCLASS(BlueprintType, Const)
class PROJECTN_API UPNPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UPNInputConfig> InputConfig;
};
