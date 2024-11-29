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
	AddTag(InputTag_Run, "InputTag.Run", "Run input.");
	AddTag(InputTag_LockOn, "InputTag.LockOn", "LockOn input.");
	AddTag(InputTag_EnableLockOn, "InputTag.EnableLockOn", "EnableLockOn input.");

	AddTag(Status_Idle, "Status.Idle", "Status Idle");
	AddTag(Status_Jump, "Status.Junp", "Status Jump");
	AddTag(Status_Run, "Status.Run", "Status Run");
	AddTag(Status_Roll, "Status.Roll", "Status Roll");
	AddTag(Status_Guard, "Status.Guard", "Status Guard");
	
	AddTag(Status_Peace, "Status.Peace", "Status Peace");
	AddTag(Status_Fight, "Status.Fight", "Status Fight");
}

void FPNGameplayTags::AddTag(FGameplayTag& OutTag, const FName TagName, const FString TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(TagName, FString(TEXT("(Native) ")) + TagComment);
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
