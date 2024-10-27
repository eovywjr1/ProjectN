// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNGameMode.h"
#include "Character/ProjectNCharacterPlayer.h"
#include "UObject/ConstructorHelpers.h"

AProjectNGameMode::AProjectNGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
