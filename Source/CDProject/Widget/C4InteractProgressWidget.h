// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C4InteractProgressWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class CDPROJECT_API UC4InteractProgressWidget : public UUserWidget
{
	GENERATED_BODY()
    
public:
	void Reset(bool isPlanting);

protected:
	UFUNCTION(BlueprintCallable)
	void SetProgress(float Progress);

	UFUNCTION(BlueprintCallable)
	void SetInteractText(const FText& NewText);
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* _interactProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* _interactText;
};
