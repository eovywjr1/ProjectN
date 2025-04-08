// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PNPlayerController.generated.h"

class UPNInputConfig;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class PROJECTN_API APNPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	bool CanCameraInputControl() const;

	void RotationByInput(const FVector2D LookAxisVector);

	void ActivateLockOn(const bool bActivate);
	void SetNextPriorityLockOnTargetActor();
	
	void UpdateLockOnCameraRotation();

private:
	APNPlayerController();

	virtual void Tick(float DeltaSeconds) override final;
	virtual void SetupInputComponent() override final;
	virtual void OnPossess(APawn* InPawn) override final;
	virtual void OnRep_Pawn() override final;

	UFUNCTION()
	void CheckLockOnTimerCallback();
	
	void SetLockOnTarget();

	void LoadInputData();

	void ToggleInventory();

private:
	APawn* LockOnTargetActor = nullptr;

	FTimerHandle CheckLockOnTimerHandle;
	FTimerHandle ClearLockOnTargetTimerHandle;
	
	float LastRotationYaw = 0.0f;

	UPROPERTY()
	TObjectPtr<UPNInputConfig> InputConfig = nullptr;
	
	UPROPERTY()
	TObjectPtr<UInputMappingContext> UIInputMappingContext = nullptr;
};
