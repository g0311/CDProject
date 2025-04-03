// Fill out your copyright notice in the Description page of Project Settings.


#include "CDHUD.h"

#include "CDProject/Widget/KDOverlay.h"
#include "Blueprint/UserWidget.h"
#include "CDProject/Widget/Announcement.h"
#include "CDProject/Widget/CharacterOverlay.h"

void ACDHUD::DrawHUD()
{
	Super::DrawHUD();
	FVector2D ViewportSize;
	if (GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter = ViewportSize * 0.5f;
		float CrosshairSpread=HUDPackage.CrosshairSpread;
		if (HUDPackage.CrosshairCenter)
		{
			FVector2D Spread(0.f,0.f);
			DrawCrosshair(HUDPackage.CrosshairCenter, Spread, HUDPackage.CrosshairColor);
		}//Getvelociy isinair
		if (HUDPackage.CrosshairRight)
		{
			FVector2D Spread(CrosshairSpread,0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairLeft)
		{
			FVector2D Spread(-CrosshairSpread,0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairTop)
		{
			FVector2D Spread(0.f,-CrosshairSpread);
			DrawCrosshair(HUDPackage.CrosshairTop, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairBottom)
		{
			FVector2D Spread(0.f,CrosshairSpread);
			DrawCrosshair(HUDPackage.CrosshairBottom, Spread, HUDPackage.CrosshairColor);
		}

	}
	
}

void ACDHUD::AddCharacterOverlay()
{
	if (APlayerController* PlayerController=GetOwningPlayerController())
	{
		CharacterOverlay=CreateWidget<UCharacterOverlay>(PlayerController,CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ACDHUD::AddGameStateOverlay()
{
	if (APlayerController* PlayerController=GetOwningPlayerController())
	{
		GameStateOverlay=CreateWidget<UKDOverlay>(PlayerController,CharacterOverlayClass);
		GameStateOverlay->AddToViewport();
	}
}

void ACDHUD::AddAnnouncement()
{
	if (APlayerController* PlayerController=GetOwningPlayerController())
	{
		Announcement=CreateWidget<UAnnouncement>(PlayerController,CharacterOverlayClass);
		Announcement->AddToViewport();
	}
}


void ACDHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ACDHUD::DrawCrosshair(UTexture2D* Texture, FVector2D Spread, FLinearColor CrosshairColor)
{
	FVector2D ViewportSize;
	if (GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D ViewportCenter = ViewportSize / 2;

	const float ScreenW=30.f;
	const float ScreenH=30.f;

	const float ScreenX=ViewportCenter.X-(ScreenW/2.f)+Spread.X;
	const float ScreenY=ViewportCenter.Y-(ScreenH/2.f)+Spread.Y;

	DrawTexture(  
	Texture,
	ScreenX,  
	ScreenY,  
	ScreenW,  
	ScreenH, 
	0.f,  
	0.f,  
	1.f,  
	1.f,  
	CrosshairColor
	);

}

