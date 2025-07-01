// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomPlayerLine.h"

#include "CDServer/Player/CDSessionPlayerController.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void URoomPlayerLine::ResetUI()
{
	TextBlock_Name->SetText(FText::GetEmpty());
	TextBlock_Ready->SetText(FText::GetEmpty());
	TextBlock_Ping->SetText(FText::GetEmpty());
	Image_HostIcon->SetVisibility(ESlateVisibility::Hidden);
	Button_Kick->SetVisibility(ESlateVisibility::Hidden);
}

void URoomPlayerLine::SetInfo(const FPlayerSessionInfo& inInfo)
{
	TextBlock_Name->SetText(FText::FromString(inInfo.Username));
	if (inInfo.ReadyState)
	{
		TextBlock_Ready->SetText(FText::FromString(TEXT("READY")));
	}
	else
	{
		TextBlock_Ready->SetText(FText::FromString(TEXT("")));
	}
	if (inInfo.bIsHost)
	{
		Image_HostIcon->SetVisibility(ESlateVisibility::Visible);
	}
	TextBlock_Ping->SetText(FText::FromString(FString::FromInt(inInfo.Ping)));

	this->Info = inInfo;
}

void URoomPlayerLine::OnKickButtonClicked()
{
	if (APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld()); IsValid(LocalPlayerController))
	{
		if (ACDSessionPlayerController* CDPC = Cast<ACDSessionPlayerController>(LocalPlayerController); IsValid(CDPC))
		{
			CDPC->Server_KickSession(Info.PlayerSessionId);
		}
	}
}
