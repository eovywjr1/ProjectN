// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "PNCharacter.generated.h"

class UPNPawnComponent;

UCLASS(config=Game)
class APNCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APNCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override final;
	
	virtual void PossessedBy(AController* NewController) override final;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	TObjectPtr<UPNPawnComponent> PawnComponent;
};
