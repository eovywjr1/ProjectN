// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PNTargetActor_HitCheckActor.h"
#include "PNTargetActor_HitCheckActor_MultiSweep.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTN_API APNTargetActor_HitCheckActor_MultiSweep : public APNTargetActor_HitCheckActor
{
	GENERATED_BODY()
	
private:
	virtual void ConfirmTargetingAndContinue() override final;
	
	void ConeTrace(TArray<AActor*>& OutHitActors) const;
};
