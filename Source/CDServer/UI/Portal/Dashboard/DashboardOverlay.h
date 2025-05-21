// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashboardOverlay.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UDashboardOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UGamePage> GamePage;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCareerPage> CareerPage;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class ULeaderBoardPage> LeaderBoardPage;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Game;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Career;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_LeaderBoard;
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void ShowGamePage();
	UFUNCTION()
	void ShowCareerPage();
	UFUNCTION()
	void ShowLeaderBoardPage();
};
