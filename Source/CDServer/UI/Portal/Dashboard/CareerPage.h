// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CareerPage.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UCareerPage : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameStatsManager> GameStatsManagerClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UMatchHistory> MatchHistoryWidget;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UPlayerStats> PlayerStatsWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Refresh;
protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY()
	TObjectPtr<class UGameStatsManager> GameStatsManager;

	UFUNCTION()
	void RefreshStatsButtonClicked();

	UFUNCTION()
	void OnRetrieveMatchStats(const FCDRetrieveMatchStatsResult& result);
};
