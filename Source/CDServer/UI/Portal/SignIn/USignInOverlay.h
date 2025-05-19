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
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPortalManager> PortalManagerClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher;
	
protected:
	UFUNCTION()
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USignInPage> SignInPage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USignUpPage> SignUpPage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UConfirmSignUpPage> ConfirmSignUpPage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USuccessConfirmedPage> SuccessConfirmedPage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_SignIn_Test;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_SignUp_Test;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_ConfirmSignUp_Test;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button_SuccessConfirm_Test;
	
	UPROPERTY()
	TObjectPtr<class UPortalManager> PortalManager;

	UFUNCTION()
	void ShowSignInPage();
	UFUNCTION()
	void ShowSignUpPage();
	UFUNCTION()
	void ShowConfirmSignUpPage();
	UFUNCTION()
	void ShowConfirmSuccessedPage();

	UFUNCTION()
	void SignInButtonClicked();
	UFUNCTION()
	void SignUpButtonClicked();
	UFUNCTION()
	void ConfirmButtonClicked();

	UFUNCTION()
	void OnSignInSucceeded();
	UFUNCTION()
	void OnSignUpSucceeded();
	UFUNCTION()
	void OnConfirmSignUpSucceeded();
};
