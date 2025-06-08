// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopOverlay.h"

#include "AIController.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameMode/RoundGameMode.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "CDProject/Types/WeaponStruct.h"
#include "Components/Button.h"
#include "Net/UnrealNetwork.h"

void UShopOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	if (RifleButton1&&SniperButton1&&SMGButton1&&PistolButton1&&ShotgunButton1)
	{
		RifleButton1->OnShopButtonClicked.AddDynamic(this, &UShopOverlay::OnShopButtonClicked);
		SniperButton1->OnShopButtonClicked.AddDynamic(this, &UShopOverlay::OnShopButtonClicked);
		PistolButton1->OnShopButtonClicked.AddDynamic(this, &UShopOverlay::OnShopButtonClicked);
		SMGButton1->OnShopButtonClicked.AddDynamic(this, &UShopOverlay::OnShopButtonClicked);
		ShotgunButton1->OnShopButtonClicked.AddDynamic(this, &UShopOverlay::OnShopButtonClicked);
	}
}

void UShopOverlay::OnShopButtonClicked(const FWeaponStruct& WeaponData)
{
	if (CanPurchase(WeaponData))
	{
		GiveWeaponToPlayer(WeaponData);
	}
	else
	{
		return;
	}
}



bool UShopOverlay::CanPurchase(const FWeaponStruct& WeaponData)
{
	PS = PS ? PS : Cast<ACDPlayerState>(GetOwningPlayerState());
	if (!PS) return false;
	if (PS && PS->GetGold() >= WeaponData.Cost)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can Purchase"));
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Purchase"));
		return false;
	}
}

void UShopOverlay::GiveWeaponToPlayer(const FWeaponStruct& WeaponData)
{
	if (!WeaponData.WeaponClass)
	{
		UE_LOG(LogTemp, Display, TEXT("No WeaponClass"));
		return;
	}
	ACDCharacter* CDCharacter = Cast<ACDCharacter>(GetOwningPlayerPawn());
	if (IsValid(CDCharacter))
		CDCharacter->ServerGiveWeapon(WeaponData);
}