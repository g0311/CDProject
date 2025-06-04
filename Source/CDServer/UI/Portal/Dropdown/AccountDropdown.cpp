// Fill out your copyright notice in the Description page of Project Settings.


#include "AccountDropdown.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "Components/TextBlock.h"

void UAccountDropdown::NativeConstruct()
{
	Super::NativeConstruct();
	
	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		TextBlock_ButtonText->SetText(FText::FromString(LocalPlayerSubsystem->Username));
	}
}

UCDLocalPlayerSubsystem* UAccountDropdown::GetLocalPlayerSubsystem()
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