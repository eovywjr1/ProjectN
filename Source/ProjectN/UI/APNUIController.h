// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PNUserWidget.h"
#include "APNUIController.generated.h"

UCLASS()
class PROJECTN_API UAPNUIController : public UPNUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
};
