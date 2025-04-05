// Fill out your copyright notice in the Description page of Project Settings.


#include "CDReloadEndAnimNotify.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Weapon/Weapon.h"

void UCDReloadEndAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp && !MeshComp->GetOwner())
		return;
	ACDCharacter* character = Cast<ACDCharacter>(MeshComp->GetOwner());
	if (character && character->IsLocallyControlled())
	{
		UCombatComponent* combat = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
		if (combat)
		{
			combat->SetFireAvail();
			combat->SetAimAvail();
			if (combat->GetCurWeapon())
				combat->GetCurWeapon()->Reload();
		}
	}
	
	Super::Notify(MeshComp, Animation);
}
