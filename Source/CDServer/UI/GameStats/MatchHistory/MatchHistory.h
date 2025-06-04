// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "MatchHistory.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UMatchHistory : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMatchHistoryLine> MatchHistoryLineClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox_History;

	void UpdateScrollBox(const TArray<FCDMatchData>& MatchData);
};
