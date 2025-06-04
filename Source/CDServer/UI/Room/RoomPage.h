// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDServer/Data/Player/FPlayerSessionInfo.h"
#include "RoomPage.generated.h"

/**
 * 
 */

UCLASS()
class CDSERVER_API URoomPage : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_RoomName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Leave;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Ready;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_ReadyButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class URoomPlayerLine> WBP_Room_Player_Line1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class URoomPlayerLine> WBP_Room_Player_Line2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class URoomPlayerLine> WBP_Room_Player_Line3;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class URoomPlayerLine> WBP_Room_Player_Line4;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class URoomPlayerLine> WBP_Room_Player_Line5;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class URoomPlayerLine> WBP_Room_Player_Line6;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UComboBoxString> Dropdown_Mode;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UComboBoxString> Dropdown_Map;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMapData> MapData;
	
	void UpdatePlayerList(const TArray<FPlayerSessionInfo> Infos, const FString& RoomMode, const FString& RoomMap);
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnLeaveButtonClicked();
	UFUNCTION()
	void OnReadyButtonClicked();
	UFUNCTION()
	void OnDropdownSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	TArray<TObjectPtr<class URoomPlayerLine>> WBP_Room_Player_Lines;
};
