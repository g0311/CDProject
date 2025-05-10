// Fill out your copyright notice in the Description page of Project Settings.


#include "CDEquipEndAnimNotify.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Weapon/Weapon.h"

void UCDEquipEndAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner())
		return;

	ACDCharacter* character = Cast<ACDCharacter>(MeshComp->GetOwner());
	if (!character)
		return;

	if (!character->IsLocallyControlled())
		return;
	
	if (!character->IsFirstPersonMesh(MeshComp))
		return;
	
	UCombatComponent* combat = character->FindComponentByClass<UCombatComponent>();
	if (!combat)
		return;

	combat->RemoveCombatState(CombatTags::State_Combat_ChangingWeapon);
	
	Super::Notify(MeshComp, Animation);
}