// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Compass.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UCompass : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SetDirection();

	UFUNCTION()
	void UpdateEnemyMarker();
	
	UFUNCTION()
	FVector2D CheckNavPoint(AActor* Target);
	
	UPROPERTY(meta = (BindWidget))
	class UImage* MarkerEnemy;

	void SetTargetEnemy(AActor* Target);
protected:
	virtual void NativeConstruct() override;

private:

	UPROPERTY(meta=(BindWidget))
	class UImage* Points;

	UPROPERTY()
	class UCanvasPanelSlot* PointsCanvasSlot;

	FTimerHandle DirectionTimer;
	FTimerHandle UpdateMarkerTimer;
	
	bool bCheckIsBehind=false;
	
	UPROPERTY()
	class ACDCharacter* CDCharacter;

	UPROPERTY()
	AActor* TargetEnemy;
	
	FRotator Rotation;
};
