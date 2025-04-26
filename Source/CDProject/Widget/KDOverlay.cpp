// Fill out your copyright notice in the Description page of Project Settings.


#include "KDOverlay.h"

#include "CDProject/GameState/CDGameState.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void UKDOverlay::UpdateScoreboard()
{
	AGameStateBase* GameState=UGameplayStatics::GetGameState(this);
	if (!GameState) return;

	const TArray<APlayerState*>& PlayerArray= GameState->PlayerArray;
	for (int32 i=0;i<PlayerArray.Num();i++)
	{
		ACDPlayerState* PS=Cast<ACDPlayerState>(PlayerArray[i]);
		if (PS&&PlayerRows.IsValidIndex(i))
		{
			PlayerRows[i]->UpdateRow(PS->GetPlayerName(),PS->GetKills(), PS->GetDeaths(), PS->GetGold());
		}
	}
}

void UKDOverlay::SetupScoreboard()
{
	if (!GetWorld() || !PlayerRowClass) return; 

	PlayerRows.Empty();
	if (RedTeamBox)
	{
		RedTeamBox->ClearChildren();
	}
	if (BlueTeamBox)
	{
		BlueTeamBox->ClearChildren();
	}
	
	AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
	if (!GameState) return;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		//ClearFunction
		ACDPlayerState* CDPS = Cast<ACDPlayerState>(PS);
		if (CDPS)
		{
			UPlayerRow* NewPlayerRow = CreateWidget<UPlayerRow>(this, PlayerRowClass);
			if (NewPlayerRow)
			{
				NewPlayerRow->Setup(CDPS);
				PlayerRows.Add(NewPlayerRow); 

				if (CDPS->GetTeam() == ETeam::ET_RedTeam)
				{
					RedTeamBox->AddChild(NewPlayerRow);
				}
				else if (CDPS->GetTeam() == ETeam::ET_BlueTeam)
				{
					BlueTeamBox->AddChild(NewPlayerRow);
				}
			}
		}
	}
}
