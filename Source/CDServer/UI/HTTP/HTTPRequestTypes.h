#pragma once

#include "CoreMinimal.h"
#include "HTTPRequestTypes.generated.h"

USTRUCT()
struct FCDMetaData
{
    GENERATED_BODY()
    
    UPROPERTY()
    int32 httpStatusCode{};
    
    UPROPERTY()
    FString requestId{};
    
    UPROPERTY()
    int32 attempts{};
    
    UPROPERTY()
    double totalRetryDelay{};
    
    void Dump()const;  
};

USTRUCT()
struct FCDListFleetsResponse
{
    GENERATED_BODY()
    
    UPROPERTY()
    TArray<FString> FleetIds{};
    
    UPROPERTY()
    FString NextToken{};
    
    void Dump()const;  
};
