#include "KDOverlay.h"

#include <string>

#include "CDProject/GameState/CDGameState.h"
#include "CDServer/Player/Team.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void UKDOverlay::SetupScoreboard()
{
	if (!GetWorld() || !PlayerRowClass) return;

	ClearScoreboard();

	ACDGameState* GameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState) return;

	ARound->SetText(FText::FromString(FString::Printf(TEXT("%d"), GameState->TeamAScore)));
	BRound->SetText(FText::FromString(FString::Printf(TEXT("%d"), GameState->TeamBScore)));

	UpdateTeamColor();
	
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
				if (CDPS->GetMatchTeam() == ETeam::ET_ATeam)
				{
					ATeamBox->AddChild(NewPlayerRow);
				}
				else if (CDPS->GetMatchTeam() == ETeam::ET_BTeam)
				{
					BTeamBox->AddChild(NewPlayerRow);
				}
			}
		}
	}
}

void UKDOverlay::UpdateTeamColor()
{
	ACDGameState* GameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState) return;

	if (GameState->IsSecondHalf)
	{
		TeamAColor->SetBrushTintColor(FColor::Blue);
		TeamBColor->SetBrushTintColor(FColor::Red);
	}
	else
	{
		TeamAColor->SetBrushTintColor(FColor::Red);
		TeamBColor->SetBrushTintColor(FColor::Blue);
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

	if (ATeamBox)
	{
		ATeamBox->ClearChildren();
	}
	if (BTeamBox)
	{
		BTeamBox->ClearChildren();
	}
}
