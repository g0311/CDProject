// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModeSelect.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UModeSelect : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Demolition;

	UPROPERTY(meta = (BindWidget))
	UButton* DeathMatch;

	UPROPERTY(EditDefaultsOnly, Category = "Game Modes")
	TSubclassOf<class AGameModeBase> DemolitionModeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game Modes")
	TSubclassOf<class AGameModeBase> DeathMatchModeClass;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnDemolitionClicked();

	UFUNCTION()
	void OnDeathMatchClicked();
};
