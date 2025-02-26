
#include "PNCommonModule.h"

bool IsClientActor(const AActor* Actor)
{
	return Actor->GetNetMode() != NM_DedicatedServer;
}