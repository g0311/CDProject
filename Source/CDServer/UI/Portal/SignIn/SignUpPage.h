// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignUpPage.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API USignUpPage : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_UserName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_Password;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_ConfirmPassword;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> TextBox_Email;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_SignUp;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_Back;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_StatusMessage;

	UFUNCTION()
	void UpdateStatusMessage(const FString& StatusMessage, bool bShouldResetWidgets);

	void ClearTextBoxes();
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void UpdateSignUpButtonState(const FText& Text);

	bool IsValidEmail(const FString& Email);
	bool IsStrongPassword(const FString& Password, FString& StatusMessage);
};
