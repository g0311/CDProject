// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePage.h"

#include "CDServer/UI/GameSessions/JoinGame/JoinGame.h"
#include "CDServer/UI/GameSessions/GameSessionsManager.h"
#include "CDServer/UI/GameSessions/PrivateSessions/PrivateSessionsWidget.h"
#include "CDServer/Data/Map/MapData.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"

void UGamePage::NativeConstruct()
{
	Super::NativeConstruct();

	check(IsValid(GameSessionManagerClass));
	GameSessionManager = NewObject<UGameSessionsManager>(this, GameSessionManagerClass);
	
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &UGamePage::JoinGameButtonClicked);
	GameSessionManager->JoinGameSessionMessageDelegate.AddDynamic(JoinGameWidget, &UJoinGame::SetStatusMessage);

	PrivateSessionsWidget->Button_Refresh->OnClicked.AddDynamic(this, &UGamePage::RefreshPrivateSessionsButtonClicked);
	GameSessionManager->OnGetSessionsRequestSucceeded.AddDynamic(PrivateSessionsWidget, &UPrivateSessionsWidget::UpdateSessions);
	RefreshPrivateSessionsButtonClicked();
	
	PrivateSessionsWidget->Button_ShowCreatePannel->OnClicked.AddDynamic(this, &UGamePage::ShowCreatePannel);
	PrivateSessionsWidget->Button_Quit->OnClicked.AddDynamic(this, &UGamePage::HideCreatePannel);
	PrivateSessionsWidget->Button_Create->OnClicked.AddDynamic(this, &UGamePage::CreatePrivateSessionButtonClicked);
	GameSessionManager->PrivateSessionCreateDelegate.AddDynamic(PrivateSessionsWidget, &UPrivateSessionsWidget::SetCreateStatusMessage);
	
	PrivateSessionsWidget->Button_Join->OnClicked.AddDynamic(this, &UGamePage::JoinPrivateSessionButtonClicked);
	GameSessionManager->PrivateSessionDelegate.AddDynamic(PrivateSessionsWidget, &UPrivateSessionsWidget::SetStatusMessage);
}

void UGamePage::JoinGameButtonClicked()
{
	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);
	PrivateSessionsWidget->Button_Join->SetIsEnabled(false);
	PrivateSessionsWidget->Button_Create->SetIsEnabled(false);
	const FString mapName = MapData->GetRandomMapFromMode(MapData->GetRandomMode());
	GameSessionManager->QuickJoinGameSession(TEXT("Demolition"), mapName);
}

void UGamePage::RefreshPrivateSessionsButtonClicked()
{
	PrivateSessionsWidget->Button_Refresh->SetIsEnabled(false);
	GameSessionManager->FindGameSessions();
}

void UGamePage::JoinPrivateSessionButtonClicked()
{
	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);
	PrivateSessionsWidget->Button_Join->SetIsEnabled(false);
	PrivateSessionsWidget->Button_Create->SetIsEnabled(false);
	UE_LOG(LogTemp, Display, TEXT("Joining private session"));
	
	if (!PrivateSessionsWidget->SelectedSessionLine)
		return;
	
	FString GameSessionId = PrivateSessionsWidget->GetCurGameSessionId();
	UE_LOG(LogTemp, Display, TEXT("Joining private session %s"), *GameSessionId);

	GameSessionManager->JoinPrivateGameSession(GameSessionId);
}

void UGamePage::CreatePrivateSessionButtonClicked()
{
	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);
	PrivateSessionsWidget->Button_Join->SetIsEnabled(false);
	PrivateSessionsWidget->Button_Create->SetIsEnabled(false);
	FString RoomName = PrivateSessionsWidget->TextBox_RoomName->GetText().ToString();
	FString RoomMode = PrivateSessionsWidget->Dropdown_Mode->GetSelectedOption();
	FString RoomMap = PrivateSessionsWidget->Dropdown_Map->GetSelectedOption();
	GameSessionManager->CreatePrivateGameSession(RoomName, RoomMode, RoomMap);
}

void UGamePage::ShowCreatePannel()
{
	PrivateSessionsWidget->CreateSessionPage->SetVisibility(ESlateVisibility::Visible);
}

void UGamePage::HideCreatePannel()
{
	PrivateSessionsWidget->CreateSessionPage->SetVisibility(ESlateVisibility::Collapsed);
	PrivateSessionsWidget->TextBox_RoomName->SetText(FText::GetEmpty());
	PrivateSessionsWidget->Dropdown_Mode->SetSelectedIndex(0);
	PrivateSessionsWidget->Dropdown_Map->SetSelectedIndex(0);
	
}