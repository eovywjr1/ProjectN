// Copyright Epic Games, Inc. All Rights Reserved.

#include "PNGameplayTags.h"

#include "GameplayTagsManager.h"
#include "PNLogChannels.h"

FPNGameplayTags FPNGameplayTags::GameplayTags;

void FPNGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(Manager);

	// Notify manager that we are done adding native tags.
	Manager.DoneAddingNativeTags();
}

void FPNGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input.");
	AddTag(InputTag_Jump, "InputTag.Jump", "Jump input.");
	AddTag(InputTag_Look, "InputTag.Look", "Look input.");

	AddMovementModeTag(MovementMode_Walking, "Movement.Mode.Walking", MOVE_Walking);
	AddTag(MovementMode_Jumping, "Movement.Mode.Jumping", "MOVE_Jumping");
}

void FPNGameplayTags::AddTag(FGameplayTag& OutTag, const FName TagName, const FString TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(TagName, FString(TEXT("(Native) ")) + TagComment);
}

void FPNGameplayTags::AddMovementModeTag(FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode)
{
	AddTag(OutTag, TagName, "Character movement mode tag.");
	GameplayTags.MovementModeTagMap.Add(MovementMode, OutTag);
}

FGameplayTag FPNGameplayTags::FindTagByString(FString TagString, bool bMatchPartialString)
{
	const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

	check(Tag.IsValid() || bMatchPartialString);

	if (Tag.IsValid() == false && bMatchPartialString)
	{
		FGameplayTagContainer AllTags;
		Manager.RequestAllGameplayTags(AllTags, true);

		for (const FGameplayTag TestTag : AllTags)
		{
			if (TestTag.ToString().Contains(TagString))
			{
				Tag = TestTag;
				break;
			}
		}
	}

	return Tag;
}
