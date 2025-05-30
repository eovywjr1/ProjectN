// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PNInteractionComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "PNGameplayTags.h"
#include "DataTable/InteractionDataTable.h"
#include "Subsystem/PNGameDataSubsystem.h"
#include "UI/PNHUD.h"

bool UPNInteractionComponent::CanInteraction() const
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetOwner<IAbilitySystemInterface>()->GetAbilitySystemComponent())
	{
		if (AbilitySystemComponent->GetGameplayTagCount(FPNGameplayTags::Get().State_DisableInteraction) > 0)
		{
			return false;
		}
	}

	if (GetInteractionDataTableKey() == NAME_None)
	{
		return false;
	}

	return true;
}

FName UPNInteractionComponent::GetInteractionDataTableKey() const
{
	// Todo. 현재 테스트 용도, 인터렉션 데이터테이블 키를 NPC/Gimmick 데이터테이블 등에서 가져와야 함
	FName InteractionDataTableKey = FName(TEXT("Test"));

	return InteractionDataTableKey;
}

void UPNInteractionComponent::OnDetectInteractableActors(TArray<AActor*>& SortedDetectedActors) const
{
	AActor* DetectedInteractableActor = nullptr;
	for (AActor* InteractableActor : SortedDetectedActors)
	{
		UPNInteractionComponent* TargetInteractionComponent = InteractableActor->FindComponentByClass<UPNInteractionComponent>();
		check(TargetInteractionComponent);

		if (!TargetInteractionComponent->CanInteraction())
		{
			continue;
		}

		DetectedInteractableActor = InteractableActor;

		break;
	}

	if (APlayerController* PlayerController = GetOwner<APawn>()->GetController<APlayerController>())
	{
		APNHUD* HUD = Cast<APNHUD>(PlayerController->GetHUD());
		
		if (DetectedInteractableActor)
		{
			const UPNInteractionComponent* TargetInteractionComponent = DetectedInteractableActor->FindComponentByClass<UPNInteractionComponent>();
			const FName TargetInteractionDataTableKey = TargetInteractionComponent->GetInteractionDataTableKey();
			check(TargetInteractionComponent->CanInteraction());

			HUD->OnDetectedInteractableActorDelegate.Broadcast(DetectedInteractableActor, TargetInteractionDataTableKey);
		}
		else
		{
			HUD->OnUnDetectedInteractableActorDelegate.Broadcast();
		}
	}
}

void UPNInteractionComponent::RequestInteraction(const FObjectKey InteractionTargetActorKey, const FName InteractionKey)
{
	AActor* InteractionTargetActor = Cast<AActor>(InteractionTargetActorKey.ResolveObjectPtr());
	if (!IsValid(InteractionTargetActor))
	{
		return;
	}

	ServerProcessInteraction(InteractionTargetActor, InteractionKey);
}

void UPNInteractionComponent::TryActivateInteractionAbility(const FGameplayTagContainer& InteractionTag) const
{
	GetOwner<IAbilitySystemInterface>()->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(InteractionTag, true);
}

UPNInteractionComponent::UPNInteractionComponent()
{
	SetIsReplicatedByDefault(true);
}

void UPNInteractionComponent::ServerProcessInteraction_Implementation(AActor* InteractionTargetActor, const FName InteractionKey)
{
	const FInteractionDataTable* InteractionDataTable = UPNGameDataSubsystem::Get(GetWorld())->GetData<FInteractionDataTable>(InteractionKey);
	if (InteractionDataTable == nullptr)
	{
		return;
	}

	if (!CanInteraction())
	{
		return;
	}

	const FGameplayTagContainer InteractionTag(InteractionDataTable->GetInteractionAbilityTag());
	InteractionTargetActor->FindComponentByClass<UPNInteractionComponent>()->TryActivateInteractionAbility(InteractionTag);
}
