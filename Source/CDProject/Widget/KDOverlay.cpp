#include "KDOverlay.h"
#include "CDProject/GameState/CDGameState.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void UKDOverlay::SetupScoreboard()
{
	if (!GetWorld() || !PlayerRowClass) return;

	ClearScoreboard();

	AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
	if (!GameState) return;

	const TArray<APlayerState*>& PlayerArray = GameState->PlayerArray;

	for (APlayerState* PS : PlayerArray)
	{
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

void UKDOverlay::ClearScoreboard()
{
	for (UPlayerRow* PlayerRow : PlayerRows)
	{
		if (PlayerRow && PlayerRow->IsValidLowLevel())
		{
			PlayerRow->RemoveFromParent();
		}
	}

	PlayerRows.Empty();

	if (RedTeamBox)
	{
		RedTeamBox->ClearChildren();
	}
	if (BlueTeamBox)
	{
		BlueTeamBox->ClearChildren();
	}
}
