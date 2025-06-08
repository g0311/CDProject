// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSlot.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void UWeaponSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (Rifle) Rifle->OnClicked.AddDynamic(this, &UWeaponSlot::OnRifleClicked);
	if (Sniper) Sniper->OnClicked.AddDynamic(this, &UWeaponSlot::OnSniperClicked);
	if (Shotgun) Shotgun->OnClicked.AddDynamic(this, &UWeaponSlot::OnShotgunClicked);
	if (SMG) SMG->OnClicked.AddDynamic(this, &UWeaponSlot::OnSMGClicked);
}

void UWeaponSlot::OnRifleClicked()
{
	OnWeaponChangeButtonClick.Broadcast("Rifle");
}

void UWeaponSlot::OnSniperClicked()
{
	OnWeaponChangeButtonClick.Broadcast("Sniper");
}

void UWeaponSlot::OnShotgunClicked()
{
	OnWeaponChangeButtonClick.Broadcast("Shotgun");
}

void UWeaponSlot::OnSMGClicked()
{
	OnWeaponChangeButtonClick.Broadcast("SMG");
}

void UWeaponSlot::SetQuickSlotMode(bool bShowQuickSlot)
{
	if (WeaponQuickSlot && WeaponQuickSlotText)
	{
		if (bShowQuickSlot)
		{
			WeaponQuickSlot->SetVisibility(ESlateVisibility::Visible);
			WeaponQuickSlotText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			WeaponQuickSlot->SetVisibility(ESlateVisibility::Collapsed);
			WeaponQuickSlotText->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
