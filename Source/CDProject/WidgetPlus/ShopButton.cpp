// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopButton.h"

#include "Components/Button.h"

void UShopButton::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button) Button->OnClicked.AddDynamic(this, &UShopButton::OnClicked);
}

void UShopButton::OnClicked()
{
	if (!WeaponDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDataTable is not assigned."));
		return;
	}
	if (RowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("RowName is not set."));
		return;
	}
	FWeaponStruct* WeaponData = WeaponDataTable->FindRow<FWeaponStruct>(RowName, TEXT("ShopButton::OnClicked"));
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid WeaponData found for RowName: %s"), *RowName.ToString());
		return;
	}
	OnShopButtonClicked.Broadcast(*WeaponData);
}
