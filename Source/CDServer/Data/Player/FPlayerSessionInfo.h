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

	UPROPERTY()
	bool bIsHost;

	UPROPERTY()
	int Index;
	
	bool operator==(const FPlayerSessionInfo& Other) const
	{
		return PlayerSessionId == Other.PlayerSessionId;
	}

	FPlayerSessionInfo(const FString& InSessionId, const FString& InUsername, bool bInReady, int32 InPing, const FString& InNetIdStr, bool bInIsHost)
	: PlayerSessionId(InSessionId), Username(InUsername), ReadyState(bInReady), Ping(InPing), NetIdStr(InNetIdStr), bIsHost(bInIsHost), Index(-1)
	{}
	FPlayerSessionInfo()
	: ReadyState(false), Ping(-1), bIsHost(false), Index(-1)
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
	
	void AddPlayer(FPlayerSessionInfo Info)
	{
		//점유 중인 인덱스 저장
		TSet<int32> UsedIndices;
		for (const FPlayerSessionInfo& Existing : Items)
		{
			UsedIndices.Add(Existing.Index);
		}

		//비점유 중 가장 낮은 인덱스 탐색
		int32 AssignedIndex = -1;
		for (int32 i = 0; i <= 5; i++)
		{
			if (!UsedIndices.Contains(i))
			{
				AssignedIndex = i;
				break;
			}
		}
		//비점유 X시 처리
		if (AssignedIndex == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("No available index for new player!"));
			return;
		}

		//Info에 인덱스 설정 후 추가
		Info.Index = AssignedIndex;
		Items.Add(Info);
		MarkArrayDirty();
	}

	void RemovePlayer(const FPlayerSessionInfo& Info)
	{
		int32 Index = Items.Find(Info);
		if (Index != INDEX_NONE)
		{
			bool IsHost = Info.bIsHost;
			Items.RemoveAt(Index);
			if (IsHost && !Items.IsEmpty())
			{ //호스트 퇴장 시 들어온 순서대로 호스트 권한 이동
				Items[0].bIsHost = true;
				Items[0].ReadyState = false;
			}
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

	bool IsPlayerHost(const FString& PlayerSessionId) const
	{
		for (auto& item : Items)
		{
			if (PlayerSessionId == item.PlayerSessionId)
			{
				if (item.bIsHost)
				{
					return true;
				}
			}
		}
		return false;
	}

	bool IsPlayerATeam(const FString& PlayerSessionId) const
	{
		for (auto& item : Items)
		{
			if (PlayerSessionId == item.PlayerSessionId)
			{
				if (item.Index >= 0 && item.Index < 3)
				{
					return true;
				}
			}
		}
		return false;
	}
	
	void Log()
	{
		UE_LOG(LogTemp, Log, TEXT("Info Log Called!!"));
		for (auto playerInfo : Items)
		{
			UE_LOG(LogTemp, Log, TEXT("username: %s, readyState: %d, ping: %d"), *playerInfo.PlayerSessionId, playerInfo.ReadyState, playerInfo.Ping);
		}
	}
};
