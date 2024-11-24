// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/PNAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "PNGameplayTags.h"

const FString PeaceString = "Status.Peace";

UPNAnimInstance::UPNAnimInstance()
	: bIsPeaceState(true)
{}

void UPNAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (AActor* Owner = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = Owner->FindComponentByClass<UAbilitySystemComponent>())
		{
			ASC->RegisterGameplayTagEvent(FPNGameplayTags::FindTagByString(PeaceString)).AddUObject(this, &UPNAnimInstance::OnPeaceTagChanged);
		}
	}
}

void UPNAnimInstance::OnPeaceTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag == FPNGameplayTags::FindTagByString(PeaceString))
	{
		bIsPeaceState = (NewCount > 0);
	}
}
