// Fill out your copyright notice in the Description page of Project Settings.


#include "CDShotGunReloadAnimNotify.h"

#include "CDAnimInstance.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"

void UCDShotGunReloadAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	
	ACDCharacter* character = Cast<ACDCharacter>(Owner);
	if (character && character->IsLocallyControlled())
	{
		UCombatComponent* combat = character->GetCombatComponent();
		if (combat)
		{
			combat->ServerShotgunReload();
			if (combat->GetCurWeapon()->GetAmmo() == combat->GetCurWeapon()->GetAmmoCapacity())
			{
				combat->ServerCancelReload();
			}
		}
	}
	Super::Notify(MeshComp, Animation);
}
