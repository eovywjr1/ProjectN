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
	AddTag(InputTag_LockOn, "InputTag.LockOn", "LockOn input.");
	AddTag(InputTag_EnableLockOn, "InputTag.EnableLockOn", "EnableLockOn input.");

	AddTag(Status_Idle, "Status.Idle", "Status Idle");
	AddTag(Status_Dead, "Status.Dead", "Status Dead");
	AddTag(Status_Peace, "Status.Peace", "Status Peace");
	AddTag(Status_Fight, "Status.Fight", "Status Fight");
	
	AddTag(Action, "Action", "Action");
	AddTag(Action_Jump, "Action.Jump", "Action Jump");
	AddTag(Action_Move, "Action.Move", "Action Move");
	AddTag(Action_Run, "Action.Run", "Action Run");
	AddTag(Action_Roll, "Action.Roll", "Action Roll");
	AddTag(Action_Guard, "Action.Guard", "Action Guard");
	AddTag(Action_Attack, "Action.Attack", "Action Attack");
	
	AddTag(GameplayEvent_AttackHitCheck, "GameplayEvent.AttackHitCheck", "GameplayEvent AttackHitCheck");
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
