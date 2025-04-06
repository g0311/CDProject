// Fill out your copyright notice in the Description page of Project Settings.


#include "CDEquipEndAnimNotify.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Weapon/Weapon.h"

void UCDEquipEndAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp && !MeshComp->GetOwner())
		return;
	ACDCharacter* character = Cast<ACDCharacter>(MeshComp->GetOwner());
	if (character)
	{
		UCombatComponent* combat = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
		if (combat)
		{
			if (character->IsLocallyControlled())
			{
				combat->SetFireAvail();
				combat->SetAimAvail();
			}
			combat->SetWeaponVisible(true);
		}
	}
	
	Super::Notify(MeshComp, Animation);
}
