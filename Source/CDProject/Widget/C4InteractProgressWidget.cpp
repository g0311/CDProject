#include "C4InteractProgressWidget.h"

#include "Components/AudioComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UC4InteractProgressWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UC4InteractProgressWidget::Reset(bool isRedTeam)
{
	SetProgress(0.f);
	SetInteractText(isRedTeam ? FText::FromString("PLANTING...") : FText::FromString("DEFUSING..."));
}

void UC4InteractProgressWidget::SetProgressTime(float Time)
{
	if (Time <= 0.f)
	{
		StopProgress();
		return;
	}
	_targetTime = 5.f;
	_progress = 0.f;
	if (!_interactAudioComponent || !_interactAudioComponent->IsPlaying())
	{
		if (_interactSound)
		{
			_interactAudioComponent = UGameplayStatics::SpawnSound2D(this, _interactSound);
		}
	}
	UpdateProgress(Time);
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

void UC4InteractProgressWidget::UpdateProgress(float CurTime)
{
	_progress = FMath::Clamp(CurTime / _targetTime, 0.f, 1.f);
	_interactProgressBar->SetPercent(_progress);

	if (_progress >= 1.f)
	{
		StopProgress();
	}
}

void UC4InteractProgressWidget::StopProgress()
{
	if (_interactAudioComponent)
		_interactAudioComponent->Stop();
}