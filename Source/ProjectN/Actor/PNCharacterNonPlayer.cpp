// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PNCharacterNonPlayer.h"

#include "Component/PNInteractionComponent.h"
#include "Component/PNActorExtensionComponent.h"

APNCharacterNonPlayer::APNCharacterNonPlayer()
{
	CreateDefaultSubobject<UPNInteractionComponent>(TEXT("InteractionComponent"));
	
	ActorExtensionComponent->ActorType = EActorType::NPC;
}
