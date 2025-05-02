#include "C4InteractProgressWidget.h"

#include "Components/AudioComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UC4InteractProgressWidget::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(_progressTimerHandle);
}

void UC4InteractProgressWidget::Reset(bool isPlanting)
{
	SetProgress(0.f);
	SetInteractText(isPlanting ? FText::FromString("PLANTING...") : FText::FromString("DEFUSING..."));
}

void UC4InteractProgressWidget::SetProgressTime(float Time)
{
	_targetTime = Time;
	_elapsedTime = 0.f;
	_progress = 0.f;
	if (_interactSound)
		_interactAudioComponent = UGameplayStatics::SpawnSound2D(this, _interactSound);
	
	GetWorld()->GetTimerManager().SetTimer(
		_progressTimerHandle,
		this,
		&UC4InteractProgressWidget::UpdateProgress,
		0.02f,
		true
	);
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

void UC4InteractProgressWidget::UpdateProgress()
{
	_elapsedTime += 0.02f;
	_progress = FMath::Clamp(_elapsedTime / _targetTime, 0.f, 1.f);
	_interactProgressBar->SetPercent(_progress);

	if (_progress >= 1.f)
	{
		StopProgress();
	}
}

void UC4InteractProgressWidget::StopProgress()
{
	GetWorld()->GetTimerManager().ClearTimer(_progressTimerHandle);
	if (_interactAudioComponent)
		_interactAudioComponent->Stop();
}