// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Types/OnlineSessionInfo.h"
#include "UOnlineObject.generated.h"

/**
 * Example of declaring a UObject in a plugin module
 */
UCLASS()
class UOnlineObject : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY()
	FOnlineSessionInfo OnlineSessionInfo;
};
