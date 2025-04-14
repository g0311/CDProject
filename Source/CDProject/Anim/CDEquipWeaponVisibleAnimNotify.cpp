// Fill out your copyright notice in the Description page of Project Settings.


#include "CDEquipWeaponVisibleAnimNotify.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Weapon/Weapon.h"

void UCDEquipWeaponVisibleAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner())
		return;

	ACDCharacter* character = Cast<ACDCharacter>(MeshComp->GetOwner());
	if (!character)
		return;
	
	if (!character->IsFirstPersonMesh(MeshComp))
		return;
	
	UCombatComponent* combat = character->GetCombatComponent();
	if (!combat)
		return;

	combat->SetBefWeaponVisible(false);
	combat->SetWeaponVisible(true);

	if (combat->GetCurWeapon())
		combat->GetCurWeapon()->SetHUDAmmo();
	
	Super::Notify(MeshComp, Animation);
}
