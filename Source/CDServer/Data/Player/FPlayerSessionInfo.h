#pragma once
#include "Net/Serialization/FastArraySerializer.h"
#include "FPlayerSessionInfo.generated.h"

USTRUCT()
struct FPlayerSessionInfo : public FFastArraySerializerItem 
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
	FString NetIdStr;
	
	bool operator==(const FPlayerSessionInfo& Other) const
	{
		return PlayerSessionId == Other.PlayerSessionId;
	}

	FPlayerSessionInfo(const FString& InSessionId, const FString& InUsername, bool bInReady, int32 InPing, const FString& InNetIdStr)
	: PlayerSessionId(InSessionId), Username(InUsername), ReadyState(bInReady), Ping(InPing), NetIdStr(InNetIdStr)
	{}
	FPlayerSessionInfo()
	: ReadyState(false), Ping(-1)
	{}
};


USTRUCT()
struct FPlayerSessionInfoArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPlayerSessionInfo> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FPlayerSessionInfo, FPlayerSessionInfoArray>(Items, DeltaParms, *this);
	}
	
	void AddPlayer(const FPlayerSessionInfo& Info)
	{
		int32 Index = Items.Add(Info);
		MarkItemDirty(Items[Index]);
	}

	void RemovePlayer(const FPlayerSessionInfo& Info)
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
				return;
			}
		}
	}

	void UpdatePing(const FString& NetIdStr, int32 Ping)
	{
		for (auto& playerInfo : Items)
		{
			if (NetIdStr == playerInfo.NetIdStr)
			{
				playerInfo.Ping = Ping;
				MarkItemDirty(playerInfo);
				return;
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
