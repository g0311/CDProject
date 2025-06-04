// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Interfaces/HUDManagement.h"
#include "PortalHUD.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API APortalHUD : public AHUD, public IHUDManagement
{
	GENERATED_BODY()
public:
	virtual void OnSignIn() override;
	virtual void OnSignOut() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USignInOverlay> SignInOverlayClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDashboardOverlay> DashboardOverlayClass;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY() 
	TObjectPtr<class USignInOverlay> SignInOverlay;
	UPROPERTY() 
	TObjectPtr<class UDashboardOverlay> DashboardOverlay;
};
