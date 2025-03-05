
#include "PNAIDataTable.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Engine/AssetManager.h"

UBehaviorTree* FPNAIDataTable::GetBehaviorTree() const
{
	return Cast<UBehaviorTree>(BehaviorTreePath.ResolveObject());
}

UBlackboardData* FPNAIDataTable::GetBlackBoard() const
{
	return Cast<UBlackboardData>(BlackBoardPath.ResolveObject());
}

void FPNAIDataTable::PostLoadDataTable()
{
	TArray<FSoftObjectPath> LoadAssets{BehaviorTreePath, BlackBoardPath};
	UAssetManager::GetStreamableManager().RequestAsyncLoad(LoadAssets);
}
