// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "WeaponSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChangeButtonClick, FName, WeaponName);


UCLASS()
class CDPROJECT_API UWeaponSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponChangeButtonClick OnWeaponChangeButtonClick;

	UFUNCTION()
	void SetQuickSlotMode(bool bShowQuickSlot);
private:
	UPROPERTY(meta=(BindWidget), meta=(AllowPrivateAccess))
	class UVerticalBox* WeaponQuickSlot;

	UPROPERTY(meta=(BindWidget), meta=(AllowPrivateAccess))
	class UTextBlock* WeaponQuickSlotText;
	
	UPROPERTY(meta=(BindWidget), meta=(AllowPrivateAccess))
	class UButton* Rifle;
	
	UPROPERTY(meta=(BindWidget), meta=(AllowPrivateAccess))
	UButton* Sniper;
	
	UPROPERTY(meta=(BindWidget), meta=(AllowPrivateAccess))
	UButton* Shotgun;
	
	UPROPERTY(meta=(BindWidget), meta=(AllowPrivateAccess))
	UButton* SMG;

	//QuickSlot Button
	UFUNCTION()
	void OnRifleClicked();
	UFUNCTION()
	void OnSniperClicked();
	UFUNCTION()
	void OnShotgunClicked();
	UFUNCTION()
	void OnSMGClicked();

	

	
	
	// UPROPERTY(meta=(BindWidget), meta=(AllowPrivateAccess))
	// UButton* Rifle;
};
