// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomPage.h"

#include "RoomPlayerLine.h"
#include "CDServer/Data/Map/MapData.h"
#include "CDServer/Data/Player/FPlayerSessionInfo.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "CDServer/Player/CDSessionPlayerController.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void URoomPage::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Leave->OnClicked.AddDynamic(this, &URoomPage::OnLeaveButtonClicked);
	Button_Ready->OnClicked.AddDynamic(this, &URoomPage::OnReadyButtonClicked);

	WBP_Room_Player_Lines.Add(WBP_Room_Player_Line1);
	WBP_Room_Player_Lines.Add(WBP_Room_Player_Line2);
	WBP_Room_Player_Lines.Add(WBP_Room_Player_Line3);
	WBP_Room_Player_Lines.Add(WBP_Room_Player_Line4);
	WBP_Room_Player_Lines.Add(WBP_Room_Player_Line5);
	WBP_Room_Player_Lines.Add(WBP_Room_Player_Line6);
	for (auto& line : WBP_Room_Player_Lines)
	{
		if (line && line->Button_Kick)
		{
			line->Button_Kick->OnClicked.AddDynamic(line, &URoomPlayerLine::OnKickButtonClicked);
		}
	}
	
	for (auto Mode : MapData->GetModes())
	{
		Dropdown_Map->AddOption(Mode);
	}
	Dropdown_Mode->SetSelectedIndex(0);
	OnDropdownSelectionChanged(FString(), ESelectInfo::Type());
	
	Dropdown_Mode->OnSelectionChanged.AddDynamic(this, &URoomPage::OnDropdownSelectionChanged);
	Dropdown_Map->OnSelectionChanged.AddDynamic(this, &URoomPage::OnDropdownSelectionChanged);
}

void URoomPage::UpdatePlayerList(const FPlayerSessionInfoArray& Infos, const FString& RoomName, const FString& RoomMode, const FString& RoomMap)
{
	//Update Player Line
 	for (int i = 0; i < WBP_Room_Player_Lines.Num(); i++)
	{
		WBP_Room_Player_Lines[i]->ResetUI();
	}
	
	for (int i = 0; i < Infos.Items.Num(); i++)
	{
		WBP_Room_Player_Lines[Infos.Items[i].Index]->SetInfo(Infos.Items[i]);
		
		WBP_Room_Player_Lines[Infos.Items[i].Index]->TextBlock_Name->SetText(FText::FromString(Infos.Items[i].Username));
		if (Infos.Items[i].ReadyState)
		{
			WBP_Room_Player_Lines[Infos.Items[i].Index]->TextBlock_Ready->SetText(FText::FromString(TEXT("READY")));
		}
		else
		{
			WBP_Room_Player_Lines[Infos.Items[i].Index]->TextBlock_Ready->SetText(FText::FromString(TEXT("")));
		}
		if (Infos.Items[i].bIsHost)
		{
			WBP_Room_Player_Lines[Infos.Items[i].Index]->Image_HostIcon->SetVisibility(ESlateVisibility::Visible);
		}
		WBP_Room_Player_Lines[Infos.Items[i].Index]->TextBlock_Ping->SetText(FText::FromString(FString::FromInt(Infos.Items[i].Ping)));
	}
	
	//Update Room Options
	TextBlock_RoomName->SetText(FText::FromString(RoomName));
	Dropdown_Mode->SetSelectedOption(RoomMode);
	Dropdown_Map->SetSelectedOption(RoomMap);
	
	//Update Room Host
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	ACDSessionPlayerController* SessionPlayerController = Cast<ACDSessionPlayerController>(LocalPlayerController);
	ULocalPlayer* LocalPlayer = LocalPlayerController->GetLocalPlayer();
	if (!Infos.Items.IsEmpty() && IsValid(SessionPlayerController) && IsValid(LocalPlayer))
	{
		if (LocalPlayer->GetSubsystem<UCDLocalPlayerSubsystem>())
		{
			const FString PlayerSessionId = SessionPlayerController->GetPlayerSessionId();
			if (Infos.IsPlayerHost(PlayerSessionId))
			{
				//Activate Select Map & Mode
				Dropdown_Mode->SetIsEnabled(true);
				Dropdown_Map->SetIsEnabled(true);
				TextBlock_ReadyButton->SetText(FText::FromString(TEXT("Start")));

				for (int i = 0; i < Infos.Items.Num(); i++)
				{
					if (!Infos.Items[i].bIsHost)
					{
						WBP_Room_Player_Lines[Infos.Items[i].Index]->Button_Kick->SetVisibility(ESlateVisibility::Visible);
					}
				}
			}
			else
			{
				//Deactivate Select Map & Mode
				Dropdown_Mode->SetIsEnabled(false);
				Dropdown_Map->SetIsEnabled(false);
				TextBlock_ReadyButton->SetText(FText::FromString(TEXT("Ready")));
			}
		}
	}
}

void URoomPage::OnLeaveButtonClicked()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		LocalPlayerController->ClientTravel(TEXT("/Game/Maps/ClientDefaultLevel"), TRAVEL_Absolute);
	}
}

void URoomPage::OnReadyButtonClicked()
{
	if (APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld()); IsValid(LocalPlayerController))
	{
		if (ACDSessionPlayerController* CDPC = Cast<ACDSessionPlayerController>(LocalPlayerController); IsValid(CDPC))
		{
			CDPC->Server_PlayerReady(false);
		}
	}
}

void URoomPage::OnDropdownSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	Dropdown_Map->ClearOptions();
	for (auto map : MapData->GetMapsFromMode(Dropdown_Mode->GetSelectedOption()))
	{
		Dropdown_Map->AddOption(map);
	}
	Dropdown_Map->SetSelectedIndex(0);
	
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{ 
		if (ACDSessionPlayerController* CDPC = Cast<ACDSessionPlayerController>(LocalPlayerController); IsValid(CDPC))
		{
			CDPC->Server_UpdateSession(Dropdown_Mode->GetSelectedOption(), Dropdown_Map->GetSelectedOption());
		}
	}
}