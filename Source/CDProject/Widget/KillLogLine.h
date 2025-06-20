// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogLine.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UKillLogLine : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Killer;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Victim;
	
protected:
	virtual void NativeConstruct() override;

private:
	FTimerHandle DestroyTimerHandle;
};
