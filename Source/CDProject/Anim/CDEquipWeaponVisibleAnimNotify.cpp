// Fill out your copyright notice in the Description page of Project Settings.


#include "CDEquipWeaponVisibleAnimNotify.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"

void UCDEquipWeaponVisibleAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner())
		return;

	ACDCharacter* character = Cast<ACDCharacter>(MeshComp->GetOwner());
	if (!character)
		return;

	if (!character->IsLocallyControlled())
		return;
	
	UCombatComponent* combat = character->FindComponentByClass<UCombatComponent>();
	if (!combat)
		return;

	int32 nextWeaponId = combat->_nextWeaponIndex;
	if (combat->GetCurWeapon())
		combat->MultiSetWeaponVisible(false);
	
	combat->ServerSetWeaponID(nextWeaponId);
	combat->MultiSetWeaponVisible(true);
	
	Super::Notify(MeshComp, Animation);
}
