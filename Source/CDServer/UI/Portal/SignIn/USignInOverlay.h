// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "USignInOverlay.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API USignInOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UJoinGame> JoinGameWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPortalManager> PortalManagerClass;

protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY()
	TObjectPtr<class UPortalManager> PortalManager;

	UFUNCTION()
	void OnJoinGameButtonClicked();
	
	UFUNCTION()
	void UpdateJoinGameStatusMessage(const FString& StatusMessage);
};
