// Fill out your copyright notice in the Description page of Project Settings.


#include "PNActorExtensionComponent.h"

#include "PNPawnData.h"
#include "AbilitySystem/PNAbilitySet.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Interface/PNAbilitySystemInterface.h"

UPNActorExtensionComponent::UPNActorExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UPNActorExtensionComponent::InitializeAbilitySystem(UPNAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor)
{
	if (AbilitySystemComponent)
	{
		return;
	}

	if (ActorType < EActorType::Player)
	{
		InAbilitySystemComponent = NewObject<UPNAbilitySystemComponent>(InOwnerActor);
	}

	if (!InAbilitySystemComponent->IsRegistered())
	{
		InAbilitySystemComponent->RegisterComponent();
	}

	AActor* Owner = GetOwner();

	AbilitySystemComponent = InAbilitySystemComponent;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Owner);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	if (Owner->HasAuthority() && PawnData)
	{
		for (const UPNAbilitySet* AbilitySet : PawnData->AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveAbilityToAbilitySystem(AbilitySystemComponent, this);
			}
		}
	}

	IPNAbilitySystemInterface* AbilitySystemInterface = GetOwner<IPNAbilitySystemInterface>();
	check(AbilitySystemInterface);
	AbilitySystemInterface->OnInitializeAbilitySystemDelegate.Broadcast();
}

void UPNActorExtensionComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const UAssetManager& AssetManager = UAssetManager::Get();
	FName PawnDataFileName;

	switch (ActorType)
	{
	case EActorType::Player:
		{
			PawnDataFileName = TEXT("DA_PlayerPawnData");
			break;
		}
	default:
		{
			break;
		}
	}
	
	if(!PawnDataFileName.IsNone())
	{
		FSoftObjectPtr AssetPtr(AssetManager.GetPrimaryAssetPath(FPrimaryAssetId(FName(TEXT("PawnData")), PawnDataFileName)));
		if (AssetPtr.IsPending())
		{
			AssetPtr.LoadSynchronous();
		}

		PawnData = Cast<UPNPawnData>(AssetPtr.Get());
		check(PawnData);
	}
}

void UPNActorExtensionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (ActorType < EActorType::Player)
	{
		InitializeAbilitySystem(nullptr, GetOwner());
	}
}
