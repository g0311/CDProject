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
	OnShopButtonClicked.Broadcast(WeaponData);
}
