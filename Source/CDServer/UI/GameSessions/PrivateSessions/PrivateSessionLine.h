// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrivateSessionLine.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UPrivateSessionLine : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_Session;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_RoomName;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_RoomCount;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_RoomMode;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_BackGround;

	TObjectPtr<class UPrivateSessionsWidget> parent;

	UFUNCTION()
	void OnSessionClicked();
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnhovered();
	void SetSelected(bool selected);
	
	void SetGameSessionId(const FString& id);
	const FString& GetGameSessionId();
private:
	FString GameSessionId;
};

