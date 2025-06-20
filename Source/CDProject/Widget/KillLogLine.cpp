// Fill out your copyright notice in the Description page of Project Settings.


#include "KillLogLine.h"

void UKillLogLine::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (GetWorld()) 
	{
		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(this))
				this->RemoveFromParent();
		}), 4.f, false);
	}
}
