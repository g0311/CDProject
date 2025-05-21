// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccountDropdown_Expanded.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UAccountDropdown_Expanded : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPortalManager> PortalManagerClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_SignOut;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_SignOutText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor HoveredTextColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor UnhoveredTextColor;
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void SignOutButton_OnClicked();
	UFUNCTION()
	void SignOutButton_Hover();
	UFUNCTION()
	void SignOutButton_Unhover();

private:
	UPROPERTY()
	TObjectPtr<class UPortalManager> PortalManager;
	
	void SetStyleTransparent();
	class UCDLocalPlayerSubsystem* GetLocalPlayerSubsystem();
};
