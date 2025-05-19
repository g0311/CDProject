// Fill out your copyright notice in the Description page of Project Settings.


#include "USignInOverlay.h"

#include "ConfirmSignUpPage.h"
#include "SignInPage.h"
#include "SignUpPage.h"
#include "SuccessConfirmedPage.h"
#include "CDServer/UI/Portal/PortalManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void USignInOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	check(PortalManagerClass);
	
	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);
	check(IsValid(PortalManager));
	
	check(Button_SignIn_Test);
	check(Button_SignUp_Test);
	check(Button_ConfirmSignUp_Test);
	check(Button_SuccessConfirm_Test);

	Button_SignIn_Test->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignInPage);
	Button_SignUp_Test->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	Button_ConfirmSignUp_Test->OnClicked.AddDynamic(this, &USignInOverlay::ShowConfirmSignUpPage);
	Button_SuccessConfirm_Test->OnClicked.AddDynamic(this, &USignInOverlay::ShowConfirmSuccessedPage);

	check(IsValid(SignInPage));
	check(IsValid(SignUpPage));
	check(IsValid(ConfirmSignUpPage));
	check(IsValid(SuccessConfirmedPage));
	
	SignInPage->Button_SignIn->OnClicked.AddDynamic(this, &USignInOverlay::SignInButtonClicked);
	SignInPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	SignInPage->Button_Quit->OnClicked.AddDynamic(PortalManager, &UPortalManager::QuitGame);
	PortalManager->SignInMessageDelegate.AddDynamic(SignInPage, &USignInPage::UpdateStatusMessage);
	PortalManager->SignInSucceededDelegate.AddDynamic(this, &USignInOverlay::OnSignInSucceeded);

	SignUpPage->Button_Back->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignInPage);
	SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::SignUpButtonClicked);
	PortalManager->SignUpMessageDelegate.AddDynamic(SignUpPage, &USignUpPage::UpdateStatusMessage);
	PortalManager->SignUpSucceededDelegate.AddDynamic(this, &USignInOverlay::OnSignUpSucceeded);
	
	ConfirmSignUpPage->Button_Confirm->OnClicked.AddDynamic(this, &USignInOverlay::ConfirmButtonClicked);
	ConfirmSignUpPage->Button_Back->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	PortalManager->ConfirmSignUpMessageDelegate.AddDynamic(ConfirmSignUpPage, &UConfirmSignUpPage::UpdateStatusMessage);
	PortalManager->ConfirmSignUpSucceededDelegate.AddDynamic(this, &USignInOverlay::OnConfirmSignUpSucceeded);

	SuccessConfirmedPage->Button_Ok->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignInPage);
}

void USignInOverlay::ShowSignInPage()
{
	check(IsValid(WidgetSwitcher));
	check(IsValid(SignInPage));

	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void USignInOverlay::ShowSignUpPage()
{
	check(IsValid(WidgetSwitcher));
	check(IsValid(SignUpPage));
	
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void USignInOverlay::ShowConfirmSignUpPage()
{
	check(IsValid(WidgetSwitcher));
	check(IsValid(ConfirmSignUpPage));
	WidgetSwitcher->SetActiveWidgetIndex(2);
}

void USignInOverlay::ShowConfirmSuccessedPage()
{
	check(IsValid(WidgetSwitcher));
	check(IsValid(SuccessConfirmedPage));
	WidgetSwitcher->SetActiveWidgetIndex(3);
}

void USignInOverlay::SignInButtonClicked()
{
	const FString Username = SignInPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignInPage->TextBox_Password->GetText().ToString();
	SignInPage->Button_SignIn->SetIsEnabled(false);
	PortalManager->SignIn(Username, Password);
}

void USignInOverlay::SignUpButtonClicked()
{
	const FString Username = SignUpPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignUpPage->TextBox_Password->GetText().ToString();
	const FString Email = SignUpPage->TextBox_Email->GetText().ToString();
	SignUpPage->Button_SignUp->SetIsEnabled(false);
	PortalManager->SignUp(Username, Password, Email);
}

void USignInOverlay::ConfirmButtonClicked()
{
	const FString ConfirmationCode = ConfirmSignUpPage->TextBox_ConfirmationCode->GetText().ToString();
	ConfirmSignUpPage->Button_Confirm->SetIsEnabled(false);
	PortalManager->Confirm(ConfirmationCode);
}

void USignInOverlay::OnSignInSucceeded()
{
	SignInPage->ClearTextBoxes();
	SignInPage->Button_SignIn->SetIsEnabled(true);
}

void USignInOverlay::OnSignUpSucceeded()
{
	SignUpPage->ClearTextBoxes();
	ConfirmSignUpPage->TextBlock_Destination->SetText(FText::FromString(PortalManager->LastSignUpResponse.CodeDeliveryDetails.Destination));
	ShowConfirmSignUpPage();
}

void USignInOverlay::OnConfirmSignUpSucceeded()
{
	ConfirmSignUpPage->ClearTextBoxes();
	ShowConfirmSignUpPage();
}
