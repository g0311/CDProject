// Fill out your copyright notice in the Description page of Project Settings.


#include "AccountDropdown_Expanded.h"

#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "CDServer/UI/Portal/PortalManager.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UAccountDropdown_Expanded::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetStyleTransparent();
	SignOutButton_Unhover();
}

void UAccountDropdown_Expanded::NativeConstruct()
{
	Super::NativeConstruct();

	Button_SignOut->OnClicked.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_OnClicked);
	Button_SignOut->OnHovered.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_Hover);
	Button_SignOut->OnUnhovered.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_Unhover);

	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);
	
	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		TextBlock_Email->SetText(FText::FromString(LocalPlayerSubsystem->Email));
	}
}

void UAccountDropdown_Expanded::SignOutButton_OnClicked()
{
	Button_SignOut->SetIsEnabled(false);
	check(PortalManager);
	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		FCDAuthenticationResult AuthenticationResult = LocalPlayerSubsystem->GetAuthResult();
		PortalManager->SignOut(AuthenticationResult.AccessToken);
	}
}

void UAccountDropdown_Expanded::SignOutButton_Hover()
{
	TextBlock_SignOutText->SetColorAndOpacity(HoveredTextColor);
}

void UAccountDropdown_Expanded::SignOutButton_Unhover()
{
	TextBlock_SignOutText->SetColorAndOpacity(UnhoveredTextColor);
}

void UAccountDropdown_Expanded::SetStyleTransparent()
{
	FButtonStyle Style;
	FSlateBrush Brush;
	Brush.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f ));
	Style.Disabled = Brush;
	Style.Hovered = Brush;
	Style.Pressed = Brush;
	Style.Normal = Brush;
	Button_SignOut->SetStyle(Style);
}

UCDLocalPlayerSubsystem* UAccountDropdown_Expanded::GetLocalPlayerSubsystem()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController) && LocalPlayerController->GetLocalPlayer())
	{
		UCDLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayerController->GetLocalPlayer()->GetSubsystem<UCDLocalPlayerSubsystem>();
		if (IsValid(LocalPlayerSubsystem))
		{
			return LocalPlayerSubsystem;
		}
	}
	return nullptr;
}
