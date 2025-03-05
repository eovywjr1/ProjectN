// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PNAIController.h"

#include "PNAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DataTable/PNAIDataTable.h"
#include "Subsystem/PNGameDataSubsystem.h"

void APNAIController::OnDetectedEnemy(const AActor* Enemy)
{
	if (Blackboard)
	{
		// 엔진 API의 non-const 필요로 const_cast를 강제로 사용
		Blackboard->SetValueAsObject(BBKEY_ENEMY, const_cast<AActor*>(Enemy));
	}
}

void APNAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Todo. 테스트 용도로 데이터테이블 키 하드코딩함, 추후 MonsterDataTable 혹은 SpawnDataTable 등에서 가져와야 함
	if (const FPNAIDataTable* AIDataTable = UPNGameDataSubsystem::Get(GetWorld())->GetData<FPNAIDataTable>(TEXT("Test")))
	{
		UBlackboardData* BlackBoard = AIDataTable->GetBlackBoard();
		UBehaviorTree* BehaviorTree = AIDataTable->GetBehaviorTree();

		if (BlackBoard && BehaviorTree)
		{
			UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
			if (UseBlackboard(BlackBoard, BlackboardComponent))
			{
				RunBehaviorTree(BehaviorTree);
			}
		}
	}
}
