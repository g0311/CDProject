// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomPage.h"

#include "RoomPlayerLine.h"
#include "CDServer/Data/Map/MapData.h"
#include "CDServer/Game/CDLobbyServerGameState.h"
#include "CDServer/Player/CDLobbyPlayerState.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
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

	for (auto Mode : MapData->GetModes())
	{
		Dropdown_Map->AddOption(Mode);
	}
	Dropdown_Mode->SetSelectedIndex(0);
	OnDropdownSelectionChanged(FString(), ESelectInfo::Type());
	
	Dropdown_Mode->OnSelectionChanged.AddDynamic(this, &URoomPage::OnDropdownSelectionChanged);
	Dropdown_Map->OnSelectionChanged.AddDynamic(this, &URoomPage::OnDropdownSelectionChanged);
}

void URoomPage::UpdatePlayerList(const TArray<FPlayerLobbyInfo> Infos, const FString& RoomMode, const FString& RoomMap)
{
	//Update Player Line
	int idx = 0;
	for (; idx < Infos.Num(); idx++)
	{
		WBP_Room_Player_Lines[idx]->TextBlock_Level->SetText(FText::FromString(TEXT("")));
		WBP_Room_Player_Lines[idx]->TextBlock_Name->SetText(FText::FromString(Infos[idx].Username));
		if (Infos[idx].ReadyState && idx != 0)
			WBP_Room_Player_Lines[idx]->TextBlock_Ready->SetText(FText::FromString(TEXT("READY")));
		else
			WBP_Room_Player_Lines[idx]->TextBlock_Ready->SetText(FText::FromString(TEXT("")));
		WBP_Room_Player_Lines[idx]->TextBlock_Ping->SetText(FText::FromString(FString::FromInt(Infos[idx].Ping)));
	}
	for (; idx < WBP_Room_Player_Lines.Num(); idx++)
	{
		WBP_Room_Player_Lines[idx]->TextBlock_Level->SetText(FText::GetEmpty());
		WBP_Room_Player_Lines[idx]->TextBlock_Name->SetText(FText::GetEmpty());
		WBP_Room_Player_Lines[idx]->TextBlock_Ready->SetText(FText::GetEmpty());
		WBP_Room_Player_Lines[idx]->TextBlock_Ping->SetText(FText::GetEmpty());
	}

	//Update Room Options
	Dropdown_Mode->SetSelectedOption(RoomMode);
	Dropdown_Map->SetSelectedOption(RoomMap);

	//Update Room Host
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	ULocalPlayer* LocalPlayer = LocalPlayerController->GetLocalPlayer();
	if (!Infos.IsEmpty() && IsValid(LocalPlayerController) && IsValid(LocalPlayer))
	{
		if (LocalPlayer->GetSubsystem<UCDLocalPlayerSubsystem>())
		{
			const FString PlayerSessionId = LocalPlayer->GetSubsystem<UCDLocalPlayerSubsystem>()->PlayerSessionId;
			if (Infos[0].PlayerSessionId == PlayerSessionId)
			{
				//Activate Select Map & Mode
				Dropdown_Mode->SetIsEnabled(true);
				//Dropdown_Map->SetIsEnabled(false);
				TextBlock_ReadyButton->SetText(FText::FromString(TEXT("Start")));
			}
			else
			{
				//Deactivate Select Map & Mode
				Dropdown_Mode->SetIsEnabled(true);
				//Dropdown_Map->SetIsEnabled(false);
				TextBlock_ReadyButton->SetText(FText::FromString(TEXT("Ready")));
			}
		}
	}
}

void URoomPage::OnLeaveButtonClicked()
{
	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (PlayerController)
	{
		PlayerController->ClientTravel(TEXT("/Game/Maps/ClientDefaultLevel"), TRAVEL_Absolute);
	}
}

void URoomPage::OnReadyButtonClicked()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		ACDLobbyPlayerState* LobbyPlayerState = LocalPlayerController->GetPlayerState<ACDLobbyPlayerState>();
		ULocalPlayer* LocalPlayer = LocalPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			const FString PlayerSessionId = LocalPlayer->GetSubsystem<UCDLocalPlayerSubsystem>()->PlayerSessionId;
			if (LobbyPlayerState)
			{
				LobbyPlayerState->Server_PlayerReady(PlayerSessionId, false);
			}
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
		ACDLobbyPlayerState* LobbyPlayerState = LocalPlayerController->GetPlayerState<ACDLobbyPlayerState>();
		ULocalPlayer* LocalPlayer = LocalPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			const FString PlayerSessionId = LocalPlayer->GetSubsystem<UCDLocalPlayerSubsystem>()->PlayerSessionId;
			if (LobbyPlayerState)
			{
				LobbyPlayerState->Server_UpdateSession(PlayerSessionId, Dropdown_Mode->GetSelectedOption(), Dropdown_Map->GetSelectedOption());
			}
		}
	}
}
