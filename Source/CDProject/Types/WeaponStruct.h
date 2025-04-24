#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WeaponStruct.generated.h"


USTRUCT()
struct FWeaponStruct: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapons)
	FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapons)
	int32 Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapons)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapons)
	FText Description;

	
};
