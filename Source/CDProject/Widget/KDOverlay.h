// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	class UTextBlock* Kill;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Death;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Gold;
};
