// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrivateSessionsWidget.generated.h"

struct FCDDescribeGameSessionResult;
/**
 * 
 */
UCLASS()
class CDSERVER_API UPrivateSessionsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPrivateSessionLine> PrivateSessionLineClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_ForSearch;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Refresh;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_ShowCreatePannel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox>ScrollBox_Sessions;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USizeBox> CreateSessionPage;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_RoomName;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UComboBoxString> Dropdown_Mode;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UComboBoxString> Dropdown_Map;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Create;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Quit;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Status;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Create_Status;
	
	TObjectPtr<class UPrivateSessionLine> SelectedSessionLine = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Join;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMapData> MapData;
	
	UFUNCTION()
	void UpdateSessions(const FCDDescribeGameSessionResult& DescribeGameSessionResult);

	FString GetCurGameSessionId();
	
	UFUNCTION()
	void SetStatusMessage(const FString& Message, bool bShouldResetWidgets);
	UFUNCTION()
	void SetCreateStatusMessage(const FString& Message, bool bShouldResetWidgets);
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void FilterScrollBox(const FText& text);
	
	UFUNCTION()
	void EnableCreateButton(const FText& Text);

	UFUNCTION()
	void OnDropdownSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};
