// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopOverlay.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "CDProject/Types/WeaponStruct.h"
#include "Components/Button.h"

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
	if (!CanPurchase(WeaponData)) return;
	GiveWeaponToPlayer(WeaponData);
}

bool UShopOverlay::CanPurchase(const FWeaponStruct& WeaponData)
{
	PS = PS ? PS : Cast<ACDPlayerState>(GetOwningPlayerState());
	if (!PS) return false;
	return PS && PS->GetGold() >= WeaponData.Cost;
}

void UShopOverlay::GiveWeaponToPlayer(const FWeaponStruct& WeaponData)
{
	if (!WeaponData.WeaponClass) return;

	Character = Character ? Character : Cast<ACDCharacter>(GetOwningPlayer());
	if (!Character) return;

	CombatComp = CombatComp ? CombatComp : Character->GetCombatComponent();
	if (!CombatComp) return;

	PC = PC ? PC : Cast<ACDPlayerController>(GetOwningPlayer());
	if (!PC || !PS) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;

	AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(
		WeaponData.WeaponClass,
		Character->GetActorLocation(),
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpawnedWeapon)
	{
		CombatComp->GetWeapon(SpawnedWeapon, true);
		PS->SpendGold(WeaponData.Cost);
	}
}
