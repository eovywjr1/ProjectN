// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"

#include "PNPawnComponent.generated.h"

UENUM()
enum class EActorType : uint8
{
	NPC,
	Player,
	MAX
};

/**
 * 
 */

class UPNAbilitySystemComponent;
class UPNPawnData;
 
UCLASS()
class PROJECTN_API UPNPawnComponent : public UPawnComponent
{
	GENERATED_BODY()
	
private:
	UPNPawnComponent(const FObjectInitializer& ObjectInitializer);
	
public:
	const UPNPawnData* GetPawnData() const { return PawnData; }
	UPNAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	void InitializeAbilitySystem(UPNAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor);
	
private:
	virtual void BeginPlay() override final;
	
public:
	EActorType ActorType = EActorType::MAX;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Pawn")
	TObjectPtr<const UPNPawnData> PawnData = nullptr;
	
	UPROPERTY()
	TObjectPtr<UPNAbilitySystemComponent> AbilitySystemComponent = nullptr;
};
