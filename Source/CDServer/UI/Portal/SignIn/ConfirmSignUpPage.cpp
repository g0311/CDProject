// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfirmSignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UConfirmSignUpPage::UpdateStatusMessage(const FString& StatusMessage, bool bShouldResetWidgets)
{
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
	if (bShouldResetWidgets)
	{
		Button_Confirm->SetIsEnabled(true);
	}
}


void UConfirmSignUpPage::ClearTextBoxes()
{
	TextBox_ConfirmationCode->SetText(FText::GetEmpty());
	TextBlock_StatusMessage->SetText(FText::GetEmpty());
	TextBlock_Destination->SetText(FText::GetEmpty());
}

void UConfirmSignUpPage::NativeConstruct()
{
	Super::NativeConstruct();

	TextBox_ConfirmationCode->OnTextChanged.AddDynamic(this, &UConfirmSignUpPage::UpdateConfirmButtonState);
}

void UConfirmSignUpPage::UpdateConfirmButtonState(const FText& Text)
{
	const FRegexPattern SixDigitPattern(TEXT(R"(^\d{6})"));
	FRegexMatcher Matcher(SixDigitPattern, Text.ToString());
	if (Matcher.FindNext())
	{
		Button_Confirm->SetIsEnabled(true);
	}
	else
	{
		Button_Confirm->SetIsEnabled(false);
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("Please enter 6 numerical digits")));
	}
}
