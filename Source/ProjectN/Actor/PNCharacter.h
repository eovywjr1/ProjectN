// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/PNActorComponentCreatorInterface.h"
#include "GameFramework/Character.h"
#include "Interface/PNAbilitySystemInterface.h"
#include "PNCharacter.generated.h"

class UPNActorGameData;
class UPNActorExtensionComponent;

UCLASS(config=Game)
class APNCharacter : public ACharacter, public IPNAbilitySystemInterface, public IPNActorComponentCreatorInterface
{
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override final;

	bool IsPlayer() const;
	bool IsRun() const;
	
	virtual UPNActorGameData* GetActorGameData() const { return nullptr; }

	void SetMaxWalkSpeed(const float InMaxSpeed);
	float GetMaxWalkSpeed() const;
	
	UFUNCTION(Server, Reliable)
	void ServerSetMaxWalkSpeed(const float InMaxSpeed);

	void OnInitializedStatus() const;

	virtual void SetDead();
	
	void SetMovable(bool bInMovable);
	FORCEINLINE bool IsMovable() const { return bMovable; }
	FORCEINLINE bool IsAttackable() const { return bMovable; }

protected:
	APNCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
private:
	virtual void PostInitializeComponents() override final;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override final;
	
	UFUNCTION(Server, Reliable)
	void ServerSetMovable(bool InIsMovable);

protected:
	UPROPERTY()
	mutable TObjectPtr<UPNActorExtensionComponent> ActorExtensionComponent;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> DeadMontage;
	
	UPROPERTY(Replicated)
	bool bMovable;
};
