// Fill out your copyright notice in the Description page of Project Settings.


#include "CDFootSoundAnimNotify.h"
#include "CDProject/Character/CDCharacter.h"

void UCDFootSoundAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ACDCharacter* character = Cast<ACDCharacter>(Owner);
	if (character)
	{
		character->PlayFootStepSound();

		//For Server Create Sound Object (for AI Perception)
		if (!character->HasAuthority())
			character->ServerPlayFootStepSound();
	}
}
