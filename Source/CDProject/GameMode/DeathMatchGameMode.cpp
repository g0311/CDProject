#include "DeathMatchGameMode.h"

#include "AIController.h"
#include "CDProject/AI/CDAIController.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameState/CDGameState.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "CDProject/Types/WeaponStruct.h"
#include "CDProject/Weapon/Weapon.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ADeathMatchGameMode::ADeathMatchGameMode()
{
	bTeamsMatch = false;
}

void ADeathMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if ( BotCount > MAX_PLAYER - _joinedClinetCount ){KickBot();}

	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		ACDPlayerState* PlayerState = NewPlayer->GetPlayerState<ACDPlayerState>();
		if (PlayerState && !BGameState->AllPlayers.Contains(PlayerState))
		{
			BGameState->AllPlayers.Add(PlayerState);
			PlayerState->SetTeam(ETeam::ET_NoTeam);
		}
	}
}

void ADeathMatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	ACDPlayerState* PlayerState = Exiting->GetPlayerState<ACDPlayerState>();
	if (BGameState && PlayerState)
	{
		BGameState->AllPlayers.Remove(PlayerState);
		UpdateAlivePlayers();
	}
}

void ADeathMatchGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	
	if ( BotCount > MAX_PLAYER - _joinedClinetCount ){KickBot();}

	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		ACDPlayerState* PlayerState = C->GetPlayerState<ACDPlayerState>();
		if (PlayerState && !BGameState->AllPlayers.Contains(PlayerState))
		{
			BGameState->AllPlayers.Add(PlayerState);
			PlayerState->SetTeam(ETeam::ET_NoTeam);
		}
	}
}

void ADeathMatchGameMode::SpawnBot()
{
	if (!AIBot) return;
	BotCount++;

	TArray<AActor*> AllStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllStarts);
	if (AllStarts.Num() == 0) return;

	const int32 Index = FMath::RandRange(0, AllStarts.Num() - 1);
	APlayerStart* ChosenStart = Cast<APlayerStart>(AllStarts[Index]);
	if (!ChosenStart) return;

	FVector SpawnLocation = ChosenStart->GetActorLocation();
	FRotator SpawnRotation = ChosenStart->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAIController* AIController = GetWorld()->SpawnActor<AAIController>(CDAIController, SpawnLocation, SpawnRotation, SpawnParams);
	if (!AIController) { BotCount--; return; }

	ACDCharacter* BotCharacter = GetWorld()->SpawnActor<ACDCharacter>(AIBot, SpawnLocation, SpawnRotation, SpawnParams);
	if (!BotCharacter) { AIController->Destroy(); BotCount--; return; }

	BotCharacter->Tags.Add(FName("Bot"));
	AIController->Possess(BotCharacter);

	ACDPlayerState* BotPlayerState = BotCharacter->GetPlayerState<ACDPlayerState>();
	if (!BotPlayerState && AIController->PlayerState)
	{
		BotPlayerState = Cast<ACDPlayerState>(AIController->PlayerState);
	}

	if (BotPlayerState)
	{
		AIController->PlayerState = BotPlayerState;
		BotPlayerState->SetOwner(AIController);
		BotPlayerState->SetUsername(TEXT("Bot") + FString::FromInt(rand()));
		BotCharacter->SetUserName(BotPlayerState->GetUsername());

		ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
		if (BGameState)
		{
			BGameState->AllPlayers.AddUnique(BotPlayerState);
		}
	}
	else
	{
		BotCharacter->Destroy();
		AIController->Destroy();
		BotCount--;
	}
}

void ADeathMatchGameMode::BalancedBot()
{
	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (!BGameState) return;

	const int32 FullCount = 10;
	const int32 CurrentPlayers = BGameState->AllPlayers.Num();
	const int32 BotsToSpawn = FullCount - CurrentPlayers;

	for (int i = 0; i < BotsToSpawn; i++)
	{
		SpawnBot();
	}

	TArray<AActor*> Bots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACDCharacter::StaticClass(), Bots);

	for (AActor* Bot : Bots)
	{
		if (!Bot->ActorHasTag("Bot")) continue;

		AAIController* BotController = Cast<AAIController>(Cast<APawn>(Bot)->GetController());
		if (!BotController) continue;

		ACDPlayerState* BotState = BotController->GetPlayerState<ACDPlayerState>();
		if (!BotState) continue;

		if (BotState->GetGold() >= 0 && ShopOverlay && WeaponDataTable)
		{
			FWeaponStruct* WeaponData = WeaponDataTable->FindRow<FWeaponStruct>(FName("Rifle"), TEXT("BotBuyWeapon"));
			if (!WeaponData) continue;

			ACDCharacter* BotCharacter = Cast<ACDCharacter>(BotController->GetCharacter());
			if (!BotCharacter) continue;

			UCombatComponent* Combat = BotCharacter->GetCombatComponent();
			if (!Combat) continue;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = BotCharacter;
			SpawnParams.Instigator = BotCharacter;

			AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponData->WeaponClass, BotCharacter->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
			if (Weapon)
			{
				Combat->GetWeapon(Weapon, true);
				BotState->SpendGold(WeaponData->Cost);
			}
		}
	}
}

void ADeathMatchGameMode::KickBot()
{
	BotCount--;

	TArray<AActor*> Bots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACDCharacter::StaticClass(), Bots);

	for (AActor* Bot : Bots)
	{
		if (!Bot->ActorHasTag("Bot")) continue;

		AController* BotController = Bot->GetInstigatorController();
		if (!BotController) continue;

		ACDPlayerState* BotState = BotController->GetPlayerState<ACDPlayerState>();
		if (!BotState) continue;

		ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
		if (BGameState)
		{
			BGameState->AllPlayers.Remove(BotState);
			UpdateAlivePlayers();
		}
		Bot->Destroy();
		return;
	}
}

void ADeathMatchGameMode::UpdateAlivePlayers()
{
	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		BGameState->AlivePlayers = BGameState->AllPlayers;
	}
}

void ADeathMatchGameMode::PlayerEliminated(class AController* VictimController, AController* AttackerController)
{
	Super::PlayerEliminated(VictimController, AttackerController);
	
	if (GetCurMatchState() == ECurMatchState::EMS_InGame)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, VictimController]()
		{
			if (!IsValid(this))
				return;
			if (VictimController)
			{
				if (ACDCharacter* Character = Cast<ACDCharacter>(VictimController->GetCharacter()))
				{
					Character->Reset();
					//Character->GiveRandomWeapon();
				
					AActor* playerStart = FindPlayerStart(VictimController);
					if (playerStart)
					{
						Character->SetActorLocation(playerStart->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
						Character->SetActorRotation(playerStart->GetActorRotation());
						VictimController->SetControlRotation(playerStart->GetActorRotation());
					}
				}
				if (ACDPlayerController* PlayerController = Cast<ACDPlayerController>(VictimController))
				{
					PlayerController->ClientSetPlayerAlive(true);
					PlayerController->ClientSetEnableInput(true);
				}
				if (ACDAIController* AIController = Cast<ACDAIController>(VictimController))
				{
					AIController->RestartBehavior();
				}
			}
		}), 1.5f, false);
	}
}

void ADeathMatchGameMode::SetCurMatchState(ECurMatchState NewState, bool IsInit)
{
	if (NewState == ECurMatchState::EMS_CoolDown)
	{
		CurRound++;
	}
	Super::SetCurMatchState(NewState, IsInit);
}

void ADeathMatchGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	BalancedBot();

	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		for (auto PlayerState : BGameState->PlayerArray)
		{
			ACDPlayerState* PS = Cast<ACDPlayerState>(PlayerState.Get());
			if (PS)
			{
				BGameState->AllPlayers.AddUnique(PS);
			}
		}
		UpdateAlivePlayers();
	}
}

void ADeathMatchGameMode::RestartMatch(bool isInit)
{
	TArray<AController*> Controllers;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AController* Controller = Cast<AController>(*It))
		{
			if (Controller->GetPawn())
			{
				Controller->GetPawn()->Destroy();
			}
			Controllers.Add(Controller);
		}
	}

	for (AController* Controller : Controllers)
	{
		RestartPlayer(Controller);
	}

	UpdateAlivePlayers();
	Super::RestartMatch(isInit);
}
