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
	
	void Setup(class ACDPlayerState* PlayerState);

private:
	UFUNCTION()
	void UpdateRow();

	UFUNCTION()
	void UpdateGold(int32 NewGold);

	ACDPlayerState* PSRef;
};
