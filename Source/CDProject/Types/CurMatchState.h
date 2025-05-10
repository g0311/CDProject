#pragma once

UENUM(BlueprintType)
enum class ECurMatchState : uint8
{
	EMS_None		UMETA(DisplayName = "None (Waitng Player)"),
	EMS_Waiting     UMETA(DisplayName = "Waiting (Shop)"),
	EMS_InGame      UMETA(DisplayName = "In Game"),
	EMS_CoolDown    UMETA(DisplayName = "Cool Down"),
	EMS_GameEnd    UMETA(DisplayName = "Match Ended")
};