// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDProject/Types/WeaponStruct.h"
#include "ShopButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopButtonClicked, const FWeaponStruct&, WeaponData);

UCLASS()
class CDPROJECT_API UShopButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponStruct WeaponData;

	UPROPERTY(BlueprintAssignable)
	FOnShopButtonClicked OnShopButtonClicked;

	virtual void NativeConstruct() override;
	
UFUNCTION()
	void OnClicked();
};
