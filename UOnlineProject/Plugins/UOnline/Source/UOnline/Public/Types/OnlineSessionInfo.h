// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "OnlineSessionInfo.generated.h"

/**
 * Online session information for both server and client.
 */
USTRUCT()
struct FOnlineSessionInfo
{
	GENERATED_USTRUCT_BODY()
 
	UPROPERTY()
	FName SessionName;

	UPROPERTY()
	FName EntryMapName;

	UPROPERTY()
	FName GameMapName;
};
