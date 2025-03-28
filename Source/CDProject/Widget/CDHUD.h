// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CDHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;
};



UCLASS()
class CDPROJECT_API ACDHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category="Player State")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY(VisibleAnywhere)
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(VisibleAnywhere)
	class UCharacterStateOverlay* StateOverlay;


	void AddCharacterOverlayOverlay();	
protected:
	virtual void BeginPlay() override;
private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D Spread, FLinearColor CrosshairColor);

	//FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) {HUDPackage=Package;}
};
