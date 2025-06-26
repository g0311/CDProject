// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDServer/Data/Player/FPlayerSessionInfo.h"
#include "RoomPlayerLine.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API URoomPlayerLine : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_HostIcon;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Name;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Ready;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Ping;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Kick;

	void ResetUI();
	void SetInfo(const FPlayerSessionInfo& inInfo);
	
	UFUNCTION()
	void OnKickButtonClicked();

private:
	FPlayerSessionInfo Info;
};
