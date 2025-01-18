// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PNAbilitySystemInterface.generated.h"

class UPNAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE(FOnInitializeAbilitySystemDelegate)

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPNAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTN_API IPNAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual UPNAbilitySystemComponent* GetAbilitySystemComponent() const = 0;
	
public:
	FOnInitializeAbilitySystemDelegate OnInitializeAbilitySystemDelegate;
};
