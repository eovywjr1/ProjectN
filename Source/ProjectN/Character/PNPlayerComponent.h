// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"

#include "PNPlayerComponent.generated.h"

/**
 * 
 */
 
struct FInputActionValue;

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class PROJECTN_API UPNPlayerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPNPlayerComponent(const FObjectInitializer& ObjectInitializer);

	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	
	virtual void BeginPlay() override final;
	
private:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Jumping(const FInputActionValue& InputActionValue);
	void Input_StopJumping(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
};
