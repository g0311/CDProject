// ShopOverlay.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDProject/WidgetPlus/ShopButton.h"
#include "ShopOverlay.generated.h"

UCLASS()
class CDPROJECT_API UShopOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnShopButtonClicked(const FWeaponStruct& WeaponData);

	
private:
	UPROPERTY(EditAnywhere, Category=Weapons)
	UDataTable* WeaponDataTable;

	UPROPERTY(meta=(BindWidget))
	UShopButton* PistolButton1;
	UPROPERTY(meta = (BindWidget))
	UShopButton* RifleButton1;
	UPROPERTY(meta=(BindWidget))
	UShopButton* SniperButton1;
	UPROPERTY(meta=(BindWidget))
	UShopButton* ShotgunButton1;
	UPROPERTY(meta=(BindWidget))
	UShopButton* SMGButton1;

	TMap<UButton*, FWeaponStruct> ButtonWeaponMap;

	class ACDPlayerState* PS;
	class ACDPlayerController* PC;
	class ACDCharacter* Character;
	class UCombatComponent* CombatComp;


	bool CanPurchase(const FWeaponStruct& WeaponData);
	void GiveWeaponToPlayer(const FWeaponStruct& WeaponData);
};