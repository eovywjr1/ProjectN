// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PNInteractionComponent.h"

#include "UI/PNHUD.h"

bool UPNInteractionComponent::CanInteraction() const
{
	// Todo. Interaction 어빌리티가 가능한지 체크해야 함

	return true;
}

FName UPNInteractionComponent::GetInteractionKey()
{
	if (!CanInteraction())
	{
		return NAME_None;
	}

	// Todo. 현재 테스트 용도, 인터렉션 데이터테이블 키를 NPC/Gimmick 데이터테이블 등에서 가져와야 함
	FName InteractionDataTableKey = FName(TEXT("Test"));
	
	return InteractionDataTableKey;
}

void UPNInteractionComponent::RequestInteraction()
{

}
