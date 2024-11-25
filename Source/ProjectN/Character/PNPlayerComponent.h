// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"

#include "PNPlayerComponent.generated.h"

/**
 * 
 */

struct FGameplayTag;
struct FInputActionValue;

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class PROJECTN_API UPNPlayerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPNPlayerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override final;
	
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	void EnableInput(bool bIsEnable) const;
	
	FVector2D GetLastMovementInput() const { return LastMovementInput; }

private:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Jumping(const FInputActionValue& InputActionValue);
	void Input_StopJumping(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	
	void Input_AbilityPressed(FGameplayTag InputTag);
	void Input_AbilityReleased(FGameplayTag InputTag);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> CameraMappingContext;
	
	FVector2D LastMovementInput;
};
