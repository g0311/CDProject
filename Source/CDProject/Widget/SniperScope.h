// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SniperScope.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API USniperScope : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* ShowScopeAnim;
};
