// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C4InteractProgressWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class CDPROJECT_API UC4InteractProgressWidget : public UUserWidget
{
	GENERATED_BODY()
    
public:
	virtual void NativeDestruct() override;
	
	void Reset(bool isPlanting);
	void SetProgressTime(float Time);

protected:
	UFUNCTION(BlueprintCallable)
	void SetProgress(float Progress);

	UFUNCTION(BlueprintCallable)
	void SetInteractText(const FText& NewText);
	void UpdateProgress();
	void StopProgress();

	UPROPERTY(meta = (BindWidget))
	UProgressBar* _interactProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* _interactText;
	
	FTimerHandle _progressTimerHandle;
	float _progress = 0.f;
	float _targetTime = 1.f;
	float _elapsedTime = 0.f;

	UPROPERTY(EditAnywhere)
	class USoundBase* _interactSound;
	UPROPERTY()
	class UAudioComponent* _interactAudioComponent;
};
