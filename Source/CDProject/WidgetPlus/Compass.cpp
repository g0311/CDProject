// Fill out your copyright notice in the Description page of Project Settings.


#include "Compass.h"

#include "Camera/CameraComponent.h"
#include "CDProject/Character/CDCharacter.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

//Compass Direction
//Widget에서는 GetOwingPlayerPawn()을 가져와야 한다.

void UCompass::NativeConstruct()
{
	Super::NativeConstruct();
	TargetEnemy=nullptr;
	
	if (APawn* Pawn=GetOwningPlayerPawn())
	{
		CDCharacter=Cast<ACDCharacter>(Pawn);
		if (CDCharacter)
		{
			SetDirection();
			
			GetWorld()->GetTimerManager().SetTimer(
				DirectionTimer,
				this,
				&UCompass::SetDirection,
				0.01f,
				true);
			
			GetWorld()->GetTimerManager().SetTimer(
				UpdateMarkerTimer,
				this,
				&UCompass::UpdateEnemyMarker,
				0.01f,
				true);
		}
		if (Points)
		{
			PointsCanvasSlot = Cast<UCanvasPanelSlot>(Points->Slot);
		}
	}
}

void UCompass::SetDirection()
{
	if (!CDCharacter || !CDCharacter->GetCamera()||!PointsCanvasSlot) return;
	
	float Yaw=CDCharacter->GetCamera()->GetComponentRotation().Yaw;;
	float OffsetX=Yaw*(-5.f);
	
	PointsCanvasSlot->SetPosition(FVector2D(OffsetX, 0.f));
}

FVector2D UCompass::CheckNavPoint(AActor* Target)//If Right XPosition<0, Left XPosition>0
{
	const FVector PlayerLocation=CDCharacter->GetActorLocation();
	const FVector NavLocation=Target->GetActorLocation();
	const FVector ToTarget=(NavLocation-PlayerLocation).GetSafeNormal2D();

	const FRotator PlayerRot=CDCharacter->GetCamera()->GetComponentRotation();
	const FVector ForwardVector=FRotationMatrix(PlayerRot).GetUnitAxis(EAxis::X);
	const FVector RightVector=FRotationMatrix(PlayerRot).GetUnitAxis(EAxis::Y);

	const float ForwardDot=FVector::DotProduct(ForwardVector.GetSafeNormal2D(),ToTarget);
	const float RightDot=FVector::DotProduct(RightVector.GetSafeNormal2D(),ToTarget);

	const float XPosition=RightDot*300.f;
	bCheckIsBehind=(ForwardDot<0);
	return FVector2D(XPosition, 0.f);
}

void UCompass::UpdateEnemyMarker()
{
	FVector2D MarkerPos;
	if (!TargetEnemy)
	{
		MarkerEnemy->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	MarkerPos.X=CheckNavPoint(TargetEnemy).X;
	if (bCheckIsBehind)
	{
		MarkerPos.Y=10.f;
	}
	else
	{
		MarkerPos.Y=200.f;
	}
	if (UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(MarkerEnemy->Slot))
	{
		MarkerSlot->SetPosition(MarkerPos);
	}
}

void UCompass::SetTargetEnemy(AActor* Target)
{
	TargetEnemy=Target;
}
