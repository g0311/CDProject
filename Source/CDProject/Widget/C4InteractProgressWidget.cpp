#include "C4InteractProgressWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UC4InteractProgressWidget::Reset(bool isPlanting)
{
	SetProgress(0.f);
	SetInteractText(isPlanting ? FText::FromString("PLANTING...") : FText::FromString("DEFUSING..."));
}

void UC4InteractProgressWidget::SetProgress(float Progress)
{
	if (_interactProgressBar)
	{
		_interactProgressBar->SetPercent(Progress);
	}
}

void UC4InteractProgressWidget::SetInteractText(const FText& NewText)
{
	if (_interactText)
	{
		_interactText->SetText(NewText);
	}
}
