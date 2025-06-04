// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "PlayerStats.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UPlayerStats : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Kill;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Death;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_HeadShot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Win;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Lose;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Draw;

	void UpdateData(const FCDRetrieveMatchStatsResult& Stats);
};
