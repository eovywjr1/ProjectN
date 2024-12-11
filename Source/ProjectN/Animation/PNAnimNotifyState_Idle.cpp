// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/PNAnimNotifyState_Idle.h"

#include "AbilitySystemComponent.h"
#include "PNGameplayTags.h"
#include "Actor/PNCharacter.h"

void UPNAnimNotifyState_Idle::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	if (APNCharacter* OwnerCharacterCast = Cast<APNCharacter>(Owner))
	{
		if (UAbilitySystemComponent* ASComponent = OwnerCharacterCast->GetAbilitySystemComponent())
		{
			ASComponent->SetLooseGameplayTagCount(FPNGameplayTags::Get().Status_Idle, 1);
		}
	}
}

void UPNAnimNotifyState_Idle::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	if (APNCharacter* OwnerCharacterCast = Cast<APNCharacter>(Owner))
	{
		if (UAbilitySystemComponent* ASComponent = OwnerCharacterCast->GetAbilitySystemComponent())
		{
			ASComponent->SetLooseGameplayTagCount(FPNGameplayTags::Get().Status_Idle, 0);
		}
	}
}
