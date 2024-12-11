// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PNCharacterPlayer.h"

#include "Component/PNEnhancedInputComponent.h"
#include "Component/PNPawnComponent.h"
#include "Component/PNPawnData.h"
#include "Component/PNPlayerComponent.h"
#include "AbilitySystem/PNAbilitySet.h"
#include "AbilitySystem/PNAbilitySystemComponent.h"
#include "Player/PNPlayerState.h"

APNCharacterPlayer::APNCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OverrideInputComponentClass = UPNEnhancedInputComponent::StaticClass();

	PlayerComponent = CreateDefaultSubobject<UPNPlayerComponent>(TEXT("PlayerComponent"));
}

void APNCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Todo. 캐릭터 스폰할 때 PawnData, AbilitySystemComponent 초기화해줘야 함
	if (APNPlayerState* PNPlayerState = GetPlayerState<APNPlayerState>())
	{
		UAbilitySystemComponent* ASComponent = PNPlayerState->GetAbilitySystemComponent();
		PawnComponent->SetAbilitySystemComponent(CastChecked<UPNAbilitySystemComponent>(ASComponent));
		ASComponent->InitAbilityActorInfo(PNPlayerState, this);

		if (const UPNPawnData* PawnData = PawnComponent->GetPawnData())
		{
			for (const UPNAbilitySet* AbilitySet : PawnData->AbilitySets)
			{
				if (AbilitySet)
				{
					AbilitySet->GiveAbilityToAbilitySystem(ASComponent, this);
				}
			}
		}
	}
}

void APNCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerComponent)
	{
		PlayerComponent->InitializePlayerInput(PlayerInputComponent);
	}
}
