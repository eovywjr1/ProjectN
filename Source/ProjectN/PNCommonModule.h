
#pragma once

UENUM(BlueprintType)
enum class EHitBoxShape : uint8
{
	Box,
	Cone
};

USTRUCT(BlueprintType)
struct FHitBoxData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	EHitBoxShape Shape;
	
	UPROPERTY(EditDefaultsOnly)
	float HitBoxDurationTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "Shape == EHitBoxShape::Cone", EditConditionHides))
	float Range = 0.0f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "Shape == EHitBoxShape::Cone", EditConditionHides))
	float Angle = 0.0f;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "Shape == EHitBoxShape::Box", EditConditionHides))
	float BoxExtent = 0.0f;
};