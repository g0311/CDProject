// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDProject/WidgetPlus/PlayerRow.h"
#include "KDOverlay.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UKDOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* RedTeamBox;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* BlueTeamBox;

	UPROPERTY(EditAnywhere, Category="Scoreboard")
	TSubclassOf<class UPlayerRow> PlayerRowClass;
	
	TArray<UPlayerRow*> PlayerRows;

	void UpdateScoreboard();
	void SetupScoreboard();
};
