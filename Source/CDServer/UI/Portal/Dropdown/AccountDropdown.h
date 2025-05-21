// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dropdown.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "AccountDropdown.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UAccountDropdown : public UDropdown
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
private:
	UCDLocalPlayerSubsystem* GetLocalPlayerSubsystem();
};
