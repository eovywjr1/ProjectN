// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PNPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "PNCommonModule.h"
#include "PNCheatManager.h"
#include "PNGameplayTags.h"
#include "Component/PNDetectComponent.h"
#include "Component/PNEnhancedInputComponent.h"
#include "Component/PNPawnSensingComponent.h"
#include "Engine/AssetManager.h"
#include "UI/PNHUD.h"

constexpr float CheckLockOnTimerPeriod = 1.0f;
constexpr float ClearLockOnTimerPeriod = 3.0f;

void APNPlayerController::UpdateLockOnCameraRotation()
{
	FRotator TargetRotation = GetControlRotation();
	TargetRotation.Yaw = (LockOnTargetActor->GetActorLocation() - PlayerCameraManager->GetCameraLocation()).Rotation().Yaw;

	SetControlRotation(TargetRotation);
}

APNPlayerController::APNPlayerController()
{
#if UE_WITH_CHEAT_MANAGER
	CheatClass = UPNCheatManager::StaticClass();
#endif
}

void APNPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsClientActor(this) && IsValid(LockOnTargetActor))
	{
		UpdateLockOnCameraRotation();
	}
}

void APNPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	LoadInputData();

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	EnhancedInputLocalPlayerSubsystem->AddMappingContext(UIInputMappingContext, 0);
	
	const FPNGameplayTags& GameplayTags = FPNGameplayTags::Get();
	UPNEnhancedInputComponent* PNEnhancedInputComponent = Cast<UPNEnhancedInputComponent>(InputComponent);

	PNEnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_ToggleInventory, ETriggerEvent::Triggered, this, &ThisClass::ToggleInventory);
}

void APNPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsClientActor(this))
	{
		// StandAlone 모드에는 리플리케이트가 되지 않기 때문에 구현
		if (UPNPawnSensingComponent* SensingComponent = GetPawn()->FindComponentByClass<UPNPawnSensingComponent>())
		{
			SensingComponent->SetPlayerSensor(PlayerCameraManager->GetFOVAngle());
		}
	}
}

void APNPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (UPNPawnSensingComponent* SensingComponent = GetPawn()->FindComponentByClass<UPNPawnSensingComponent>())
	{
		SensingComponent->SetPlayerSensor(PlayerCameraManager->GetFOVAngle());
	}
}

bool APNPlayerController::CanCameraInputControl() const
{
	if (IsValid(LockOnTargetActor))
	{
		return false;
	}

	return true;
}

void APNPlayerController::RotationByInput(const FVector2D LookAxisVector)
{
	if (CanCameraInputControl() == false)
	{
		return;
	}

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void APNPlayerController::ActivateLockOn(const bool bActivate)
{
	ESenseType SenseType = bActivate ? ESenseType::LockOn : ESenseType::Default;
	GetPawn()->FindComponentByClass<UPNPawnSensingComponent>()->ConvertSenseType(SenseType);

	if (bActivate)
	{
		SetLockOnTarget();
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckLockOnTimerHandle);
		LockOnTargetActor = nullptr;
		
		Cast<APNHUD>(GetHUD())->OnDeactivatedLockOnDelegate.Broadcast();
	}
}

void APNPlayerController::SetNextPriorityLockOnTargetActor()
{
	GetPawn()->FindComponentByClass<UPNDetectComponent>()->SetTargetNextPriorityEnemy();
	SetLockOnTarget();
}

void APNPlayerController::CheckLockOnTimerCallback()
{
	// 현재 LockOnTarget이 TargetDetectedEnemy와 같지 않다면 락온 조건에서 벗어난 경우
	const AActor* TargetDetectedEnemy = GetPawn()->FindComponentByClass<UPNDetectComponent>()->GetTargetedEnemy();
	if (LockOnTargetActor != TargetDetectedEnemy)
	{
		GetWorld()->GetTimerManager().SetTimer(ClearLockOnTargetTimerHandle, [this]() { ActivateLockOn(false); }, ClearLockOnTimerPeriod, false, ClearLockOnTimerPeriod);
	}
}

void APNPlayerController::SetLockOnTarget()
{
	UPNDetectComponent* DetectComponent = GetPawn()->FindComponentByClass<UPNDetectComponent>();
	APawn* LockOnTarget = DetectComponent->GetTargetedEnemy();

	LockOnTargetActor = LockOnTarget;

	if (LockOnTargetActor && CheckLockOnTimerHandle.IsValid() == false)
	{
		GetWorld()->GetTimerManager().SetTimer(CheckLockOnTimerHandle, this, &ThisClass::CheckLockOnTimerCallback, CheckLockOnTimerPeriod, true, CheckLockOnTimerPeriod);
	}

	if (LockOnTargetActor)
	{
		Cast<APNHUD>(GetHUD())->OnSetLockOnTargetDelegate.Broadcast(LockOnTargetActor);
	}
}

void APNPlayerController::LoadInputData()
{
	const UAssetManager& AssetManager = UAssetManager::Get();
	if (InputConfig == nullptr)
	{
		FSoftObjectPtr AssetPtr(AssetManager.GetPrimaryAssetPath(FPrimaryAssetId(FName(TEXT("PlayerInput")), FName(TEXT("DA_PlayerInputConfig")))));
		if (AssetPtr.IsPending())
		{
			AssetPtr.LoadSynchronous();
		}

		InputConfig = Cast<UPNInputConfig>(AssetPtr.Get());
		check(InputConfig);
	}
	
	if (UIInputMappingContext == nullptr)
	{
		FSoftObjectPtr AssetPtr(AssetManager.GetPrimaryAssetPath(FPrimaryAssetId(FName(TEXT("PlayerInput")), FName(TEXT("IMC_UI")))));
		if (AssetPtr.IsPending())
		{
			AssetPtr.LoadSynchronous();
		}

		UIInputMappingContext = Cast<UInputMappingContext>(AssetPtr.Get());
		check(UIInputMappingContext);
	}
}

void APNPlayerController::ToggleInventory()
{
	APNHUD* HUD = Cast<APNHUD>(GetHUD());
	HUD->ToggleInventory();
}
