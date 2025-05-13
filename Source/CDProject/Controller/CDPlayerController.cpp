// Fill out your copyright notice in the Description page of Project Settings.


#include "CDPlayerController.h"

#include <filesystem>

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Character/CDCharacterAttributeSet.h"
#include "CDProject/Component/CDSpringArmComponent.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/GameMode/RoundGameMode.h"
#include "CDProject/GameState/CDGameState.h"
#include "CDProject/HUD/CDHUD.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Widget/Announcement.h"
#include "CDProject/Widget/C4InteractProgressWidget.h"
#include "CDProject/Widget/CharacterOverlay.h"
#include "CDProject/Widget/KDOverlay.h"
#include "CDProject/Widget/SniperScope.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/SpringArmComponent.h"
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
	DOREPLIFETIME(ACDPlayerController, HUDGoldCount);
	DOREPLIFETIME(ACDPlayerController, HUDDeathCount);
	DOREPLIFETIME(ACDPlayerController, HUDKillCount);
	DOREPLIFETIME(ACDPlayerController, WaitingStartTime);
	DOREPLIFETIME(ACDPlayerController, MatchStartTime);
	DOREPLIFETIME(ACDPlayerController, CooldownStartTime);
}

void ACDPlayerController::ServerCheckMatchState_Implementation()
{
	ARoundGameMode* GameMode=Cast<ARoundGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime=GameMode->WarmUpTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->WaitingStartTime;
		CooldownTime=GameMode->CooldownTime;
		MatchState = GameMode->GetCurMatchState();
		if (!IsLocalController())
			ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
		else
			ClientJoinMidgame_Implementation(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}
//GameMode is accessible only on the server

void ACDPlayerController::ClientJoinMidgame_Implementation(ECurMatchState StateOfMatch, float Warmup, float Match,
	float Cooldown,float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	CooldownTime=Cooldown;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (CDHUD && MatchState == ECurMatchState::EMS_Waiting)
	{
		CDHUD->AddAnnouncement();
	}
}

void ACDPlayerController::ClientSetMatchTime_Implementation(float matchTime)
{
	MatchTime = matchTime;
}

void ACDPlayerController::ClientSetMatchState_Implementation(ECurMatchState state, float curTime)
{
	MatchState = state;
	OnMatchStateSet(MatchState);
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
	PS = Cast<ACDPlayerState>(GetPlayerState<ACDPlayerState>());
	if (PS)
	{
		PS->OnGoldUpdated.AddDynamic(this, &ACDPlayerController::SetGold);
	}
	ServerCheckMatchState();
	if (IsLocalController())
	{
		UpdateCharacterOverlay();
		CDHUD->AddAnnouncement();
	}
}

float ACDPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();// 서버라면 시간 반환
	else return GetWorld()->GetTimeSeconds()+ClientServerDelta;//클라이언트라면 보정값 반환
}

void ACDPlayerController::ShowKDOverlay(bool isShowing)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		if (isShowing)
		{
			CDHUD->AddKDOverlay(true);
		}
		else
		{
			CDHUD->AddKDOverlay(false);
		}
	}
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

void ACDPlayerController::HandleWaiting()
{
	if (!IsLocalController())
		return;
	if (IsValid(GetPawn()))
	{
		GetPawn()->DisableInput(this);
		SetShowMouseCursor(true);
	}
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		ShowStoreWidget(true);

		if (CDHUD->Announcement&&CDHUD->Announcement->AnnouncementText&&CDHUD->Announcement->AnnouncementCountdown)
		{
			CDHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("");
			CDHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
		}
	}
}

void ACDPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if (!IsLocalController())
		return;
	if (IsValid(GetPawn()))
	{
		GetPawn()->EnableInput(this);
		SetShowMouseCursor(false);
	}
	
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		ShowStoreWidget(false);

		if (CDHUD->Announcement)
		{
			CDHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ACDPlayerController::HandleCooldown()
{
	if (!IsLocalController())
		return;
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		if (CDHUD->Announcement&&CDHUD->Announcement->AnnouncementText&&CDHUD->Announcement->AnnouncementCountdown)
		{
			CDHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("");
			CDHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
		}
	}
}

void ACDPlayerController::ServerSendClientJoined_Implementation()
{
	if (GetWorld()->GetAuthGameMode())
	{
		ARoundGameMode* gamemode = Cast<ARoundGameMode>(GetWorld()->GetAuthGameMode());
		if (IsValid(gamemode))
		{
			gamemode->SendPlayerJoined();
		}
	}
}


void ACDPlayerController::SetHUDHealth(float Health)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay)
	{
		const float HealthPercent = Health/100.f;
		CDHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText=FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
		CDHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeHealth=true;
	}
}

void ACDPlayerController::SetHUDShield(float Shield)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&CDHUD->CharacterOverlay)
	{
		const float ShieldPercent = Shield/100.f;
		CDHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		// FString HealthText=FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		// CDHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeShield=true;
	}
	
}


void ACDPlayerController::SetHUDWeaponAmmo(int32 WeaponAmmo)
{
	if (CDHUD&&CDHUD->CharacterOverlay && CDHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		FString WeaponAmmoText = FString::Printf(TEXT("%d"), WeaponAmmo);
		CDHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
		//CDHUD->AddCompass();//here!/ if you want to deactivate Compass UI, annotation this!
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
	else
	{
		bInitializeWeaponInfo=true;
	}
}


void ACDPlayerController::SetHUDWeaponCarriedAmmo(int32 CarriedAmmo)
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
	
	if (MatchState == ECurMatchState::EMS_Waiting)
	{
		TimeLeft = WaitingStartTime + WarmupTime - GetServerTime();
	}
	else if (MatchState == ECurMatchState::EMS_InGame)
	{
		TimeLeft = MatchStartTime + MatchTime - GetServerTime();
	}
	else if (MatchState == ECurMatchState::EMS_CoolDown)
	{
		TimeLeft = CooldownStartTime + CooldownTime - GetServerTime();
	}
	else if (MatchState == ECurMatchState::EMS_None || MatchState == ECurMatchState::EMS_GameEnd)
	{
		TimeLeft = 0.f;
	}
	
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	
	if (CountdownInt!=SecondsLeft)
	{
		SetHUDMatchCount(TimeLeft);
	}
	CountdownInt=SecondsLeft;
}

void ACDPlayerController::UpdateCharacterOverlay()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (!CDHUD)
		return;
	
	CDHUD->AddCharacterOverlay();
	SetGold();
	if (OwnedCharacter)
	{
		SetHUDWeaponAmmo(OwnedCharacter->GetCombatComponent()->GetCurAmmo());
		SetHUDWeaponCarriedAmmo(OwnedCharacter->GetCombatComponent()->GetCarriedAmmo());
		SetHUDWeaponInfo(OwnedCharacter->GetCombatComponent()->GetCurWeapon());
		SetHUDHealth(OwnedCharacter->GetAttributeSet()->GetHealth());
		SetHUDShield(OwnedCharacter->GetAttributeSet()->GetShield());
	}
	SetMinimap();
}

void ACDPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD && !CDHUD->Announcement)
	{
		CDHUD->AddAnnouncement();
	}
	
	if (CDHUD&&CDHUD->Announcement&&CDHUD->Announcement->AnnouncementCountdown)
	{
		int32 Sec = CountdownTime;
		FString CountdownText = FString::Printf(TEXT("%d"), Sec);
		//UE_LOG(LogTemp,Display,TEXT("%s"),*CountdownText);
		CDHUD->Announcement->AnnouncementCountdown->SetText(FText::FromString(CountdownText));
		if (CountdownTime<0.f)
		{
			CDHUD->Announcement->AnnouncementCountdown->SetText(FText());
		}
	}
	
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

void ACDPlayerController::SetGold(int32 NewGold)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	PS=PS==nullptr?Cast<ACDPlayerState>(GetPlayerState<ACDPlayerState>()):PS;
	
	// if (CDHUD && CDHUD->CharacterOverlay && PS)
	// {
	// 	HUDGoldCount = PS->GetGold();
	// 	FText GoldText = FText::AsNumber(HUDGoldCount); 
	// 	CDHUD->CharacterOverlay->Gold->SetText(GoldText);
	// }
	if (CDHUD && CDHUD->CharacterOverlay && PS)
	{
		HUDGoldCount = NewGold;
		FText GoldText = FText::AsNumber(HUDGoldCount); 
		CDHUD->CharacterOverlay->Gold->SetText(GoldText);
	}
	else
	{
		bInitializeGold=true;
	}
}

void ACDPlayerController::SetKDOverlayUI()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&KDOverlay)
	{
		CDHUD->KDOverlay->SetupScoreboard();
	}
}

void ACDPlayerController::UpdateKDOverlayData()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD&&KDOverlay)
	{
		CDHUD->KDOverlay->SetupScoreboard();
	}
}

void ACDPlayerController::Client_ShowStoreWidget_Implementation(bool IsActivate)
{
	ShowStoreWidget(IsActivate);
}


void ACDPlayerController::ShowStoreWidget(bool bActivate)
{
	if (!IsLocalController()) return;
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		CDHUD->AddStore(bActivate);
	}
	else
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("RetryShowStoreWidget"), bActivate);
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDel);
		//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACDPlayerController::ShowStoreWidget(bActivate));
	}
}
void ACDPlayerController::RetryShowStoreWidget(bool bActivate)
{
	ShowStoreWidget(bActivate);
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

void ACDPlayerController::SetHUDATeam(int32 RedScore)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD && CDHUD->KDOverlay)
	{
		FString ScoreText=FString::Printf(TEXT("%d"), RedScore);
		//CDHUD->KDOverlay->SetupScoreboard();
		CDHUD->KDOverlay->ARound->SetText(FText::FromString(ScoreText));
	}
	
	// if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->RedTeamScore&&CDHUD->KDOverlay)
	// {
	// 	FString ScoreText=FString::Printf(TEXT("%d"), RedScore);
	// 	CDHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	// 	CDHUD->KDOverlay->ARound->SetText(FText::FromString(ScoreText));
	// }
}

void ACDPlayerController::SetHUDBTeam(int32 BlueScore)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD && CDHUD->KDOverlay)
	{
		FString ScoreText=FString::Printf(TEXT("%d"), BlueScore);
		//CDHUD->KDOverlay->SetupScoreboard();
		CDHUD->KDOverlay->BRound->SetText(FText::FromString(ScoreText));
	}
	
	// CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	// if (CDHUD&&CDHUD->CharacterOverlay&&CDHUD->CharacterOverlay->BlueTeamScore&&CDHUD->KDOverlay)
	// {
	// 	FString ScoreText=FString::Printf(TEXT("%d"), BlueScore);
	//CDHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	// 	CDHUD->KDOverlay->BRound->SetText(FText::FromString(ScoreText));
	// }
}

void ACDPlayerController::SetTeamUIColor()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD && CDHUD->KDOverlay)
	{
		CDHUD->KDOverlay->UpdateTeamColor();
	}
}

void ACDPlayerController::ShowAnnounceText(bool bShow)
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	if (CDHUD)
	{
		if (bShow)
		{
			CDHUD->AddAnnouncement();
			if (CDHUD->Announcement&&CDHUD->Announcement->AnnouncementText&&CDHUD->Announcement->AnnouncementCountdown)
			{
				FString AnnouncementText = "Starting Match...";
				CDHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
				CDHUD->Announcement->AnnouncementCountdown->SetText(FText());
			}
		}
		else
		{
			CDHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ACDPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	if (IsLocalController()) 
	{
		ServerSendClientJoined();
		
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

		acdCharacter->GetSpringArmComponent()->bUsePawnControlRotation = true;

		OwnedCharacter = acdCharacter;
	}
}

void ACDPlayerController::OnMatchStateSet(ECurMatchState State, bool bTeamsMatch, float time)
{
	MatchState=State;
	if (MatchState==ECurMatchState::EMS_Waiting)
	{
		WaitingStartTime = time;
		GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
	else if (MatchState==ECurMatchState::EMS_InGame)
	{
		MatchStartTime = time;
		GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else if (MatchState==ECurMatchState::EMS_CoolDown)
	{
		CooldownStartTime = time;
	}
	else if (MatchState==ECurMatchState::EMS_GameEnd)
	{
		//?
	}
	
	if (IsLocalController() && HasAuthority()) //for Listen Server
		OnRep_MatchState();
}

void ACDPlayerController::OnRep_MatchState()
{
	if (MatchState==ECurMatchState::EMS_Waiting)
	{
		HandleWaiting();
		//Show Store HUD
	}
	else if (MatchState==ECurMatchState::EMS_InGame)
	{
		//Hide Store HUD
		HandleMatchHasStarted();
	}
	else if (MatchState==ECurMatchState::EMS_CoolDown)
	{
		HandleCooldown();
	}
	else if (MatchState == ECurMatchState::EMS_GameEnd)
	{
		//Show Game End UI
		
		
		// if (IsLocalController())
		// {
		// 	UGameplayStatics::OpenLevel(this, FName("Menu"));
		// }
	}
}

void ACDPlayerController::OnRep_HUDGoldCount()
{
	int32 NewGold=0;
	SetGold(NewGold);
}

void ACDPlayerController::OnRep_HUDKillCount()
{
	
}

void ACDPlayerController::OnRep_HUDDeathCount()
{
	
}

void ACDPlayerController::LeaveGame()
{
	//temp
	UGameplayStatics::OpenLevel(this, FName("Menu"));
	//Super::LeaveGame();
}

void ACDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (enhancedInputComponent)
	{
		enhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ACDPlayerController::LMouseDown);
	}
}

void ACDPlayerController::ClientSetPlayerAlive_Implementation(bool isAlive)
{
	if (!isAlive)
	{
		UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()); 
		if (subSystem)
		{
			subSystem->AddMappingContext(DeadInputMappingContext, 1);
		}
		if(ACDGameState* GameState = Cast<ACDGameState>(GetWorld()->GetGameState()))
		{
			for (int i = 0; i < GameState->PlayerArray.Num(); i++)
			{
				if (ACDCharacter* character = Cast<ACDCharacter>(GameState->PlayerArray[i]->GetPawn()))
				{
					OwnedCharacter = OwnedCharacter==nullptr ? Cast<ACDCharacter>(GetCharacter()) : OwnedCharacter;
					if (OwnedCharacter && character->GetTeam() == OwnedCharacter->GetTeam())
					{
						TeamCharacters.Push(character);
						if (character == OwnedCharacter)
						{
							CurPlayerIndex = i;
						}
					}
				}
			}	
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RESET RESET CALLBACK"));
		UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()); 
		if (subSystem)
		{
			subSystem->RemoveMappingContext(DeadInputMappingContext);
		}
		TeamCharacters.Empty();
		CurPlayerIndex = 0;
		if (OwnedCharacter)
			SetViewTarget(OwnedCharacter);
	}
}

void ACDPlayerController::LMouseDown()
{
	if (TeamCharacters.Num() == 0) return;

	int32 StartIndex = CurPlayerIndex;
	do
	{
		CurPlayerIndex = (CurPlayerIndex + 1) % TeamCharacters.Num();

		if (TeamCharacters[CurPlayerIndex] && !TeamCharacters[CurPlayerIndex]->_isDead)
		{
			SetViewTarget(TeamCharacters[CurPlayerIndex]);
			return;
		}
	} 
	while (CurPlayerIndex != StartIndex);
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

void ACDPlayerController::ShowC4PlantingProgress(bool isPlanting, float time)
{
	CDHUD = CDHUD == nullptr ? Cast<ACDHUD>(GetHUD()) : CDHUD;
	if (CDHUD == nullptr) return;

	if (!CDHUD->C4InteractProgress)
	{
		CDHUD->AddC4Progress();
	}
	if (CDHUD && CDHUD->C4InteractProgress)
	{
		CDHUD->C4InteractProgress->Reset(true);
		if (isPlanting)
		{
			CDHUD->C4InteractProgress->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CDHUD->C4InteractProgress->SetVisibility(ESlateVisibility::Hidden);
		}
		CDHUD->C4InteractProgress->SetProgressTime(time);
	}
}
void ACDPlayerController::ShowC4DefusingProgress(bool isDefusing, float time)
{
	CDHUD = CDHUD == nullptr ? Cast<ACDHUD>(GetHUD()) : CDHUD;
	if (CDHUD == nullptr) return;

	if (CDHUD && CDHUD->C4InteractProgress)
	{
		CDHUD->C4InteractProgress->Reset(false);
		if (isDefusing)
		{
			CDHUD->C4InteractProgress->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CDHUD->C4InteractProgress->SetVisibility(ESlateVisibility::Hidden);
		}
		CDHUD->C4InteractProgress->SetProgressTime(time);
	}
}