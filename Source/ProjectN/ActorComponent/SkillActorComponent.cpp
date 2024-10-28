// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USkillActorComponent::USkillActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// Enable Skill Flag
	EnableSkillFlags.Add(ESkillType::Roll, true);

	// CoolDown TimerHandle
	FTimerHandle RollCollDownTimerHandle;
	SkillCoolDownTimerHandles.Add(ESkillType::Roll, RollCollDownTimerHandle);

	// Todo. 데이터에서 애니메이션 경로 가져와야 함
	if (UAnimMontage* RollMontage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/ProjectN/Animation/AM_Roll")))
	{
		SkillActionMontages.Add(ESkillType::Roll, RollMontage);
	}
}

void USkillActorComponent::RequestSkill(const int32 SkillTypeIndex)
{
	ACharacter* Owner = GetOwner<ACharacter>();
	if (Owner == nullptr)
		return;

	const ESkillType SkillType = static_cast<ESkillType>(SkillTypeIndex);

	if (EnableSkillFlags[SkillType] == false)
		return;

	EnableSkillFlags[SkillType] = false;

	ProcessSkill(SkillType);

	// 쿨다운 타이머
	// Todo. 추후 데이터로 쿨다운 수치 가져와야 함
	const float RollCooldown = 2.0f;

	GetWorld()->GetTimerManager().SetTimer(SkillCoolDownTimerHandles[SkillType], FTimerDelegate::CreateLambda([this, SkillType]()
		{
			EnableSkillFlags[SkillType] = true;
		}), RollCooldown,false);
}

bool USkillActorComponent::CheckEnableSkill(const ESkillType SkillType) const
{
	return EnableSkillFlags[SkillType];
}

void USkillActorComponent::ProcessSkill(const ESkillType SkillType)
{
	PlaySkillMontage(SkillType);
}

void USkillActorComponent::PlaySkillMontage(const ESkillType SkillType)
{
	UAnimInstance* AnimInstance = GetOwner<ACharacter>()->GetMesh()->GetAnimInstance();

	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(SkillActionMontages[SkillType]);
}
