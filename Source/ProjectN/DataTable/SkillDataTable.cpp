
#include "SkillDataTable.h"

#include "GameplayEffect.h"
#include "AbilitySystem/AttributeSet/PNPlayerAttributeSet.h"

void FSkillDataTable::ApplySkillModifiers(UGameplayEffect& InSkillEffect) const
{
	{
		FGameplayModifierInfo StatusModifierInfo;
		StatusModifierInfo.Attribute = UPNPawnAttributeSet::GetAttackDamageAttribute();
		StatusModifierInfo.ModifierOp = EGameplayModOp::Override;
		StatusModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(GetSkillDamage()));
		InSkillEffect.Modifiers.Add(StatusModifierInfo);
	}

	{
		FGameplayModifierInfo StatusModifierInfo;
		StatusModifierInfo.Attribute = UPNPlayerAttributeSet::GetSRAttribute();
		const int8 SkillEffectSRAmount = GetGetSkillResource() - GetUseSkillResource();
		StatusModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(SkillEffectSRAmount));
		InSkillEffect.Modifiers.Add(StatusModifierInfo);
	}
}
