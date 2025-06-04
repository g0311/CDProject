// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignInPage.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API USignInPage : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void UpdateStatusMessage(const FString& StatusMessage, bool bShouldResetWidgets);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_UserName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_Password;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_SignIn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_SignUp;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_Quit;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_StatusMessage;
	
	void ClearTextBoxes();
};
