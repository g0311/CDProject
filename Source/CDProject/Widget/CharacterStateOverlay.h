// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStateOverlay.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UCharacterStateOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* KillCount;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DeathCount;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Score;
};
