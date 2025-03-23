// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UCDCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCDCharacterAttributeSet, Health, OldHealth);
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