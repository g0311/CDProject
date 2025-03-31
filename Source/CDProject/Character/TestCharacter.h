// MyCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TestCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UAnimMontage;

UCLASS()
class ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATestCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Movement */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	/** Combat */
	void Equip(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);

	/** Enhanced Input */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Equip;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Fire;

	/** Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EquipMontage;
};
