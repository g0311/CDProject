// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePage.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UGamePage : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameSessionsManager> GameSessionManagerClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UJoinGame> JoinGameWidget;
	
protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY()
	TObjectPtr<class UGameSessionsManager> GameSessionManager;
	
	UFUNCTION()
	void JoinGameButtonClicked();
};
