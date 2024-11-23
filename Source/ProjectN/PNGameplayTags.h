// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "GameplayTagContainer.h"
#include "HAL/Platform.h"

class UGameplayTagsManager;

/**
 * FPNGameplayTags
 *
 *	Singleton containing native gameplay tags.
 */
 
struct FPNGameplayTags
{
public:
	static const FPNGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	static FGameplayTag FindTagByString(FString TagString, bool bMatchPartialString = false);

public:
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Look;

	FGameplayTag MovementMode_Idle;
	FGameplayTag MovementMode_Walking;
	FGameplayTag MovementMode_Jumping;

	TMap<uint8, FGameplayTag> MovementModeTagMap;
	TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

private:
	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const FName TagName, const FString TagComment);
	void AddMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode);

private:
	static FPNGameplayTags GameplayTags;
};
