// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SuccessConfirmedPage.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API USuccessConfirmedPage : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_Ok;
}; 
