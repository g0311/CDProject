// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerRow.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UPlayerRow : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNameText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* KillText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DeathText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* GoldText;

	void UpdateRow(FText PlayerName, int32 Kills, int32 Deaths, int32 Gold);
	void Setup(class ACDPlayerState* Player);
};
