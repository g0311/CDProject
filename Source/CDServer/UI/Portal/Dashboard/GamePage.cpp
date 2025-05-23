// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePage.h"

#include "CDServer/UI/GameSessions/JoinGame/JoinGame.h"
#include "CDServer/UI/GameSessions/GameSessionsManager.h"
#include "CDServer/UI/GameSessions/PrivateSessions/PrivateSessionsWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UGamePage::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(GameSessionManagerClass))
	{
		GameSessionManager = NewObject<UGameSessionsManager>(this, GameSessionManagerClass);
	}
	
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &UGamePage::JoinGameButtonClicked);
	GameSessionManager->JoinGameSessionMessageDelegate.AddDynamic(JoinGameWidget, &UJoinGame::SetStatusMessage);

	PrivateSessionsWidget->Button_Refresh->OnClicked.AddDynamic(this, &UGamePage::RefreshPrivateSessionsButtonClicked);
	GameSessionManager->OnGetSessionsRequestSucceeded.AddDynamic(PrivateSessionsWidget, &UPrivateSessionsWidget::UpdateSessions);
	RefreshPrivateSessionsButtonClicked();

	PrivateSessionsWidget->Button_ShowCreatePannel->OnClicked.AddDynamic(this, &UGamePage::ShowCreatePannel);
	PrivateSessionsWidget->Button_Quit->OnClicked.AddDynamic(this, &UGamePage::HideCreatePannel);
	PrivateSessionsWidget->Button_Create->OnClicked.AddDynamic(this, &UGamePage::CreatePrivateSessionButtonClicked);
	GameSessionManager->PrivateSessionDelegate.AddDynamic(PrivateSessionsWidget, &UPrivateSessionsWidget::SetStatusMessage);
	
	PrivateSessionsWidget->Button_Join->OnClicked.AddDynamic(this, &UGamePage::JoinPrivateSessionButtonClicked);
	GameSessionManager->PrivateSessionCreateDelegate.AddDynamic(PrivateSessionsWidget, &UPrivateSessionsWidget::SetCreateStatusMessage);
}

void UGamePage::JoinGameButtonClicked()
{
	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);
	GameSessionManager->JoinGameSession(TEXT("Demolition"));
}

void UGamePage::RefreshPrivateSessionsButtonClicked()
{
	PrivateSessionsWidget->Button_Refresh->SetIsEnabled(false);
	GameSessionManager->FindGameSessions();
}

void UGamePage::JoinPrivateSessionButtonClicked()
{
	PrivateSessionsWidget->Button_Join->SetIsEnabled(false);
	FString GameSessionId = PrivateSessionsWidget->GetCurGameSessionId();
	GameSessionManager->JoinPrivateGameSession(GameSessionId);
}

void UGamePage::CreatePrivateSessionButtonClicked()
{
	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);
	FString RoomName = PrivateSessionsWidget->TextBox_RoomName->GetText().ToString();
	FString RoomMode = PrivateSessionsWidget->Dropdown_GameMode->GetSelectedOption();
	FString RoomMap = TEXT("Default");
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
	PrivateSessionsWidget->Dropdown_GameMode->SetSelectedIndex(0);
	
}

void UPrivateSessionsWidget::SetStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	TextBlock_Status->SetText(FText::FromString(Message));
	if (bShouldResetWidgets)
	{
		Button_Join->SetIsEnabled(true);
	}
}

void UPrivateSessionsWidget::SetCreateStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	TextBlock_Create_Status->SetText(FText::FromString(Message));
	if (bShouldResetWidgets)
	{
		Button_Create->SetIsEnabled(true);
	}
}
