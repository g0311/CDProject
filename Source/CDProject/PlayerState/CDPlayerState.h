#pragma once

#include "CoreMinimal.h"
#include "CDProject/Types/Team.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "GameFramework/PlayerState.h"

#include "CDPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldUpdated, int32, NewGold);


UCLASS()
class CDPROJECT_API ACDPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ACDPlayerState();

    virtual void BeginPlay() override;
    
    void AddKill();
    void AddDeath();
    void AddGold(int32 Amount);
    void SetGold(int32 Amount);
    bool SpendGold(int32 Amount);
    
    int32 GetKills() const { return Kills; }
    int32 GetDeaths() const { return Deaths; }
    FString GetPlayerName() const { return Name; }
    int32 GetGold() const { return Gold; }
    ETeam GetTeam() const { return Team; }
    ETeam GetMatchTeam() const { return MatchTeam; }

    void AddShot() { TotalShot++; }
    void AddHeadShot() { HeadShot++;}
    
    void SetTeam(ETeam NewTeam);
    void SetMatchTeam(ETeam NewTeam);
    void SwitchTeam();
    
    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreUpdated OnScoreUpdated;
    UPROPERTY(BlueprintAssignable, Category = "Gold")
    FOnGoldUpdated OnGoldUpdated;

    FCDMatchStats GetRecordInput();
protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION()
    void OnRep_Team();
    UFUNCTION()
    void OnRep_Gold();
    UFUNCTION()
    void OnRep_Kills();
    UFUNCTION()
    void OnRep_Deaths();

private:
    UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, Category = "Player Stats")
    ETeam Team = ETeam::ET_NoTeam;
    UPROPERTY(Replicated, VisibleAnywhere)
    ETeam MatchTeam = ETeam::ET_NoTeam;
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Player Stats")
    FString Name;
    UPROPERTY(ReplicatedUsing = OnRep_Gold, EditAnywhere, Category = "Player Stats")
    int32 Gold = 0;

    UPROPERTY(ReplicatedUsing = OnRep_Kills, VisibleAnywhere, Category = "Player Stats")
    int32 Kills = 0;
    UPROPERTY(ReplicatedUsing = OnRep_Deaths, VisibleAnywhere, Category = "Player Stats")
    int32 Deaths = 0;
    UPROPERTY(VisibleAnywhere)
    int32 TotalShot = 0;
    UPROPERTY(VisibleAnywhere)
    int32 HeadShot = 0;
};
