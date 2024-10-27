// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillActorComponent.generated.h"

UENUM()
enum class ESkillType : uint8
{
	Roll,
	Count
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTN_API USkillActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillActorComponent();

	UFUNCTION()
	void RequestSkill(const int32 SkillTypeIndex);
	bool CheckEnableSkill(const ESkillType SkillType) const;

private:
	virtual void BeginPlay() override final;

	void ProcessSkill(const ESkillType SkillType);
	void PlaySkillMontage(const ESkillType SkillType);

private:
	TMap<ESkillType, bool> EnableSkillFlags;
	TMap<ESkillType, FTimerHandle> SkillCoolDownTimerHandles;

	// 추후에 Info 쪽으로 옮겨야함
	UPROPERTY()
	TMap<ESkillType, TObjectPtr<UAnimMontage>> SkillActionMontages;
};
