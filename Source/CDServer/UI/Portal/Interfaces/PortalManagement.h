// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PortalManagement.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPortalManagement : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CDSERVER_API IPortalManagement
{
	GENERATED_BODY()
public:
	virtual void RefreshToken(const FString& RefreshToken) = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
