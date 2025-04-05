// Fill out your copyright notice in the Description page of Project Settings.


#include "CDEquipEndAnimNotify.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Weapon/Weapon.h"

void UCDEquipEndAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp && !MeshComp->GetOwner())
		return;

	UCombatComponent* combat = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
	if (combat)
	{
		combat->SetFireAvail();
		combat->SetAimAvail();
		//combat->GetCurWeapon()->Reload();
	}
	
	Super::Notify(MeshComp, Animation);
}
