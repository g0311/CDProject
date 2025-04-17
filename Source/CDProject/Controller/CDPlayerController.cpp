// Fill out your copyright notice in the Description page of Project Settings.


#include "CDPlayerController.h"

#include <filesystem>

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Character/CDCharacterAttributeSet.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/GameMode/CDGameMode.h"
#include "CDProject/HUD/CDHUD.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Widget/Announcement.h"
#include "CDProject/Widget/CharacterOverlay.h"
#include "CDProject/Widget/KDOverlay.h"
#include "CDProject/Widget/SniperScope.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ACDPlayerController::ACDPlayerController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACDPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetHUDTime();
	//InitializeHUD();
	CheckTimeSync(DeltaSeconds);
}

void ACDPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACDPlayerController, MatchState);
}

void ACDPlayerController::ServerCheckMatchState_Implementation()
{
	ACDGameMode* GameMode=Cast<ACDGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime=GameMode->WarmUpTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		CooldownTime=GameMode->CooldownTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}
//GameMode is accessible only on the server

void ACDPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match,
	float Cooldown,float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	CooldownTime=Cooldown;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (CDHUD && MatchState == MatchState::WaitingToStart)
	{
		CDHUD->AddAnnouncement();
	}
}

void ACDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CDHUD=Cast<ACDHUD>(GetHUD());
	ACDCharacter* _character = Cast<ACDCharacter>(GetCharacter());
	if (CDHUD && _character)
	{
		if (bInitializeHealth)
		{
			SetHUDHealth(_character->GetAttributeSet()->GetHealth());
		}
		if (bInitializeShield)
		{
			SetHUDShield(_character->GetAttributeSet()->GetShield());
		}
	}

	ServerCheckMatchState();
}

float ACDPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();// 서버라면 시간 반환
	else return GetWorld()->GetTimeSeconds()+ClientServerDelta;//클라이언트라면 보정값 반환
}

void ACDPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}//접속하면 자신의 시간 서버에 보고
}


void ACDPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt=GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}
//접속한 모든 클라들은 서버에 접속하면 서버에게 현재시간 보고, 서버는 서버시간, 해당 클라시간을 같이 클라에 보냄.
void ACDPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
                                                                 float TimeServerReceivedClientRequest)
{
	float RoundTripTime=GetWorld()->GetTimeSeconds()-TimeOfClientRequest;
	float CurrentServerTime=TimeServerReceivedClientRequest+(0.5f*RoundTripTime);
	ClientServerDelta=CurrentServerTime-GetWorld()->GetTimeSeconds();
}
//RTT*0.5해서 서버 예측시간을 클라의 현재시간에 반환. GetServerTime을 요청한 클라는 Delta를 통해 해당시간 동기화 가능해짐.

void ACDPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime+=DeltaTime;
	if (IsLocalController()&&TimeSyncRunningTime>TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime=0.f;
	}
}


void ACDPlayerController::HandleCooldown()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		CDHUD->CharacterOverlay->RemoveFromParent();
		if (CDHUD->Announcement&&CDHUD->Announcement->AnnouncementText&&CDHUD->Announcement->AnnouncementCountdown)
		{
			CDHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			CDHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			CDHUD->Announcement->AnnouncementCountdown->SetText(FText());
		}
	}
}


void ACDPlayerController::SetHUDHealth(float Health)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("SETHEALTH2"));
		const float HealthPercent = Health/100.f;
		CDHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText=FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
		CDHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SETHEALTH1_1"));

		bInitializeHealth=true;
	}
}

void ACDPlayerController::SetHUDShield(float Shield)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay)
	{
		// const float ShieldPercent = Shield/MaxShield;
		// //CDHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		// FString HealthText=FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		// CDHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	
}


// void ACDPlayerController::SetHUDKill(float killcount)
// {
// 	if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->KillCount)
// 	{
// 		FString KillCount=FString::Printf(TEXT("%d"), FMath::CeilToInt(killcount));
// 		CDHUD->CharacterOverlay->KillCount->SetText(FText::FromString(KillCount));
// 	}
// }
//
// void ACDPlayerController::SetHUDDeath(float deathcount)
// {
// 	if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->KDOverlay->DeathCount)
// 	{
// 		FString DeathCount=FString::Printf(TEXT("%d"), FMath::CeilToInt(deathcount));
// 		CDHUD->CharacterOverlay->DeathCount->SetText(FText::FromString(DeathCount));
// 	}
// }

void ACDPlayerController::SetHUDWeaponAmmo(int32 WeaponAmmo)
{
	if (CDHUD&&CDHUD->CharacterOverlay && CDHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		FString WeaponAmmoText = FString::Printf(TEXT("%d"), WeaponAmmo);
		CDHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
	}
	else
	{
		bInitializeWeaponAmmo=true;
	}
}

void ACDPlayerController::SetHUDWeaponInfo(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;

	if (CDHUD&&CDHUD->CharacterOverlay && CDHUD->CharacterOverlay)
	{
		FString WeaponName = Weapon->WeaponName;
		if (!WeaponName.IsEmpty() && CDHUD->CharacterOverlay->WeaponName)
		{
			CDHUD->CharacterOverlay->WeaponName->SetText(FText::FromString(WeaponName));
		}

		UTexture2D* WeaponImage = Weapon->WeaponImage;
		if (WeaponImage && CDHUD->CharacterOverlay->WeaponImage)
		{
			CDHUD->CharacterOverlay->WeaponImage->SetBrushFromTexture(WeaponImage);
		}
	}
}

void ACDPlayerController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	if (CDHUD&&CDHUD->CharacterOverlay && CDHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		FString CarriedText = FString::Printf(TEXT("%d"), CarriedAmmo);
		CDHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedText));
	}
	else
	{
		bInitializeCarriedAmmo=true;
	}
}

void ACDPlayerController::SetHUDMatchCount(float CountdownTime)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->MatchCountdownText)
	{
		if (CountdownTime<0.f)
		{
			CDHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Min=FMath::FloorToInt(CountdownTime/60);
		int32 Sec = FMath::FloorToInt(FMath::Fmod(CountdownTime, 60.f));
		FString CountdownText = FString::Printf(TEXT("%d:%d"), Min,Sec);
		CDHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
	
}
void ACDPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (CountdownInt!=SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart||MatchState==MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCount(TimeLeft);
		}
		
	}
	CountdownInt=SecondsLeft;
}

void ACDPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->Announcement&&CDHUD->Announcement->AnnouncementCountdown)
	{
		if (CountdownTime<0.f)
		{
			CDHUD->Announcement->AnnouncementCountdown->SetText(FText());
			return;
		}
		int32 Sec = FMath::Fmod(CountdownTime, 60.f);
		FString CountdownText = FString::Printf(TEXT("%d"), Sec);
		CDHUD->Announcement->AnnouncementCountdown->SetText(FText::FromString(CountdownText));
	}
	
}

void ACDPlayerController::SetTeamScore()
{
	
}

void ACDPlayerController::SetMinimap()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	APawn* CDPawn = GetPawn();
	if (!CDPawn)return;
	ACDCharacter* CDCharacter = Cast<ACDCharacter>(CDPawn);
	if (!CDCharacter) return;
	
	if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->MinimapBox)
	{
		if (CDHUD->CharacterOverlay->MiniMapImage)
		{
			UTextureRenderTarget2D* MiniMapRenderTarget=CDCharacter->GetMiniMapTarget();
			FSlateBrush MiniMapBrush;
			MiniMapBrush.SetResourceObject(MiniMapRenderTarget);
			MiniMapBrush.ImageSize = FVector2D(128, 128);
			
			CDHUD->CharacterOverlay->MiniMapImage->SetBrush(MiniMapBrush);
			
			
		}
	}
}


//120 -> 119 -> 118
void ACDPlayerController::InitializeHUD()
{
	if (CharacterOverlay)
	{
		// if (bInitializeHealth)SetHUDHealth(HUDHealth, HUDMaxHealth);
		// if (bInitializeCarriedAmmo)SetHUDCarriedAmmo(HUDCarriedAmmo);
		// if (bInitializeWeaponAmmo)SetHUDWeaponAmmo(HUDWeaponAmmo);
		//if (bInitializeWeaponAmmo)SetHUDWeaponInfo(HUDWeaponInfo);
		//if (bInitializeShield)SetHUDShield(HUDShield);
		// if (bInitializeKill)SetHUDKill(HUDKillCount);
		// if (bInitializeDeath)SetHUDDeath(HUDDeathCount);
	}
}



void ACDPlayerController::HideRoundScore(bool IsHide)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->StateBox&&IsHide)
	{
		CDHUD->CharacterOverlay->StateBox->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		CDHUD->CharacterOverlay->StateBox->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACDPlayerController::SetHUDRedTeam(int32 RedScore)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->RedTeamScore)
	{
		FString ScoreText=FString::Printf(TEXT("%d"), RedScore);
		CDHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
	
}

void ACDPlayerController::SetHUDBlueTeam(int32 BlueScore)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->RedTeamScore)
	{
		FString ScoreText=FString::Printf(TEXT("%d"), BlueScore);
		CDHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void ACDPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	
	EnableInput(this); 

	if (IsLocalController()) 
	{
		UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()); 
		ACDCharacter* acdCharacter = dynamic_cast<ACDCharacter*>(P);
		if (subSystem && acdCharacter)
		{
			subSystem->AddMappingContext(acdCharacter->GetInputMapping(), 0);
		}
		if (acdCharacter->GetAbilitySystemComponent())
		{
			acdCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(P, P);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("SETHEALTH1"));
		SetHUDHealth(acdCharacter->GetAttributeSet()->GetHealth());
		SetHUDShield(acdCharacter->GetAttributeSet()->GetShield());
	}
}

void ACDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);



}


void ACDPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState=State;
	if (MatchState==MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState==MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ACDPlayerController::OnRep_MatchState()
{
	if (MatchState==MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState==MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


void ACDPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		CDHUD->AddCharacterOverlay();
		SetMinimap();
		if (CDHUD->Announcement)
		{
			CDHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}



void ACDPlayerController::ShowSniperScope()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (!CDHUD->SniperScope)
	{
		CDHUD->AddSniperScope();
	}

	if (CDHUD&&CDHUD->SniperScope&&CDHUD->SniperScope->ScopeZoomIn)
	{
		ACDCharacter* CDCharacter=Cast<ACDCharacter>(GetCharacter());
		if (CDCharacter->GetCombatComponent()->IsAiming()) 
		{
			CDHUD->SniperScope->PlayAnimation(CDHUD->SniperScope->ScopeZoomIn);
		}
		else
		{
			CDHUD->SniperScope->PlayAnimation(CDHUD->SniperScope->ScopeZoomIn, 0.f,1,EUMGSequencePlayMode::Reverse);
		}
	}
}
