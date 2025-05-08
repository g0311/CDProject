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
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RedRound;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* BlueRound;
protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* RedTeamBox;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* BlueTeamBox;

	UPROPERTY(EditAnywhere, Category = "Scoreboard")
	TSubclassOf<UPlayerRow> PlayerRowClass;

private:
	TArray<UPlayerRow*> PlayerRows;

	void ClearScoreboard();
};
