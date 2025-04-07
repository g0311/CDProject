// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacterAttributeSet.h"

#include "CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Net/UnrealNetwork.h"

void UCDCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCDCharacterAttributeSet, Health, OldHealth);

	UE_LOG(LogTemp, Warning, TEXT("OnRep_Health Called"));

	if (AActor* Owner = GetOwningActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Got Owning Actor: %s"), *Owner->GetName());
		if (ACDCharacter* CDCharacter = Cast<ACDCharacter>(Owner))
		{
			UE_LOG(LogTemp, Warning, TEXT("Casted to CDCharacter: %s"), *CDCharacter->GetName());
			if (APlayerController* PC = Cast<APlayerController>(CDCharacter->GetController()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Got PlayerController"));
				if (ACDPlayerController* CDPC = Cast<ACDPlayerController>(PC))
				{
					UE_LOG(LogTemp, Warning, TEXT("Set HUD Health"));
					CDPC->SetHUDHealth(GetHealth(), GetMaxHealth());
				}
			}
		}
	}
}

void UCDCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCDCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void UCDCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCDCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCDCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}