// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacterAttributeSet.h"

#include "CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Net/UnrealNetwork.h"

void UCDCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCDCharacterAttributeSet, Health, OldHealth);
	if (AActor* Owner = GetOwningActor())
	{
		if (ACDCharacter* CDCharacter = Cast<ACDCharacter>(Owner))
		{
			if (APlayerController* PC = Cast<APlayerController>(CDCharacter->GetController()))
			{
				if (ACDPlayerController* CDPC = Cast<ACDPlayerController>(PC))
				{
					CDPC->SetHUDHealth(GetHealth());
				}
			}
		}
	}
}

void UCDCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCDCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void UCDCharacterAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
		UE_LOG(LogTemp, Log, TEXT("OnRep_Sh_Called!!"));
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCDCharacterAttributeSet, Shield, OldShield);
	if (AActor* Owner = GetOwningActor())
	{
		if (ACDCharacter* CDCharacter = Cast<ACDCharacter>(Owner))
		{
			if (APlayerController* PC = Cast<APlayerController>(CDCharacter->GetController()))
			{
				if (ACDPlayerController* CDPC = Cast<ACDPlayerController>(PC))
				{
					CDPC->SetHUDShield(GetHealth());
				}
			}
		}
	}
}

void UCDCharacterAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCDCharacterAttributeSet, MaxShield, OldMaxShield);
}

void UCDCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCDCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCDCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCDCharacterAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCDCharacterAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
}
