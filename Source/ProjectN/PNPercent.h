#pragma once

#include "CoreMinimal.h"
#include "PNPercent.generated.h"

UENUM()
enum class EPercentType : uint32
{
	E1Percent   = 10000,
	E10Percent  = E1Percent * 10,
	E100Percent = E10Percent * 100,
};

USTRUCT(BlueprintType)
struct FPNPercent
{
	GENERATED_BODY()

public:
	explicit FPNPercent() = default;

	explicit FPNPercent(float InValue)
	{
		RawValue = FMath::Clamp(InValue * static_cast<int32>(EPercentType::E1Percent), 0, static_cast<int32>(EPercentType::E100Percent));
	}
	
	static FPNPercent FromFraction(int32 Numerator, int32 Denominator);
	
	FORCEINLINE bool operator==(const FPNPercent& Other) const { return RawValue == Other.RawValue; }
	FORCEINLINE bool operator<(const FPNPercent& Other) const { return RawValue < Other.RawValue; }
	FORCEINLINE bool operator>(const FPNPercent& Other) const { return RawValue > Other.RawValue; }

private:
	int32 RawValue = 0;
};
