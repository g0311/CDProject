#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CDProject/WidgetPlus/PlayerRow.h"
#include "KDOverlay.generated.h"

UCLASS()
class CDPROJECT_API UKDOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupScoreboard();
	void UpdateTeamColor();
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ARound;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* BRound;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* TeamAColor;
	UPROPERTY(meta = (BindWidget))
	UImage* TeamBColor;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ATeamBox;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* BTeamBox;

	UPROPERTY(EditAnywhere, Category = "Scoreboard")
	TSubclassOf<UPlayerRow> PlayerRowClass;

private:
	UPROPERTY()
	TArray<UPlayerRow*> PlayerRows;
	void ClearScoreboard();
};
