#pragma once
#include "Net/Serialization/FastArraySerializer.h"
#include "FPlayerLobbyInfo.generated.h"

USTRUCT()
struct FPlayerLobbyInfo : public FFastArraySerializerItem 
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlayerSessionId = TEXT("");

	UPROPERTY()
	FString Username;

	UPROPERTY()
	bool ReadyState;

	UPROPERTY()
	int32 Ping;
	
	UPROPERTY()
	FString NetIDStr;
	
	bool operator==(const FPlayerLobbyInfo& Other) const
	{
		return PlayerSessionId == Other.PlayerSessionId;
	}

	FPlayerLobbyInfo(const FString& InSessionId, const FString& InUsername, bool bInReady, int32 InPing, const FString& InNetIdStr)
	: PlayerSessionId(InSessionId), Username(InUsername), ReadyState(bInReady), Ping(InPing), NetIDStr(InNetIdStr)
	{}
	FPlayerLobbyInfo()
	: ReadyState(false), Ping(-1)
	{}
};


USTRUCT()
struct FPlayerLobbyInfoArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPlayerLobbyInfo> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FPlayerLobbyInfo, FPlayerLobbyInfoArray>(Items, DeltaParms, *this);
	}
	
	void AddPlayer(const FPlayerLobbyInfo& Info)
	{
		int32 Index = Items.Add(Info);
		MarkItemDirty(Items[Index]);
	}

	void RemovePlayer(const FPlayerLobbyInfo& Info)
	{
		int32 Index = Items.Find(Info);
		if (Index != INDEX_NONE)
		{
			Items.RemoveAt(Index);
			MarkArrayDirty();
		}
	}

	void UpdatePlayerReadyState(const FString& PlayerSessionId, bool bShouldReset)
	{
		for (auto& playerInfo : Items)
		{
			if (PlayerSessionId == playerInfo.PlayerSessionId)
			{
				if (bShouldReset)
				{
					playerInfo.ReadyState = false;
				}
				else
				{
					playerInfo.ReadyState = !playerInfo.ReadyState;
				}
				MarkItemDirty(playerInfo);
			}
		}
	}

	bool IsAllPlayerReady()
	{
		for (int i = 1; i < Items.Num(); i++)
		{
			if (!Items[i].ReadyState)
			{
				return false;
			}
		}
		return true;
	}

	bool IsPlayerHost(const FString& PlayerSessionId)
	{
		if (Items[0].PlayerSessionId == PlayerSessionId)
		{
			return true;
		}
		return false;
	}

	void Log()
	{
		for (auto playerInfo : Items)
		{
			UE_LOG(LogTemp, Log, TEXT("username: %s, readyState: %d, ping: %d"), *playerInfo.PlayerSessionId, playerInfo.ReadyState, playerInfo.Ping);
		}
	}
};
