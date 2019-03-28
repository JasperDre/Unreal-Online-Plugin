// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UOnlineObject.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"

UOnlineObject::UOnlineObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOnlineObject::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UOnlineObject::OnStartSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UOnlineObject::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOnlineObject::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UOnlineObject::OnDestroySessionComplete);
	OnReadFriendsListCompleteDelegate = FOnReadFriendsListComplete::CreateUObject(this, &UOnlineObject::OnReadFriendsListComplete);
	OnSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UOnlineObject::OnSessionUserInviteAccepted);
}

bool UOnlineObject::CreateSession(TSharedPtr<const FUniqueNetId> arg_UserNetId, FName arg_SessionName, FName arg_Map, bool arg_bIsLAN, bool arg_bIsPresence, int32 arg_MaxNumPlayers)
{
	// Get the Online Subsystem to work with
	const IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();

	if (OnlineSubsystemInterface)
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();

		if (OnlineSessionInterface.IsValid() && arg_UserNetId.IsValid())
		{
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = arg_bIsLAN;
			SessionSettings->bUsesPresence = arg_bIsPresence;
			SessionSettings->NumPublicConnections = arg_MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			if (!arg_Map.ToString().IsEmpty())
			{
				SessionSettings->Set(SETTING_MAPNAME, arg_Map, EOnlineDataAdvertisementType::ViaOnlineService);
			}

			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return OnlineSessionInterface->CreateSession(*arg_UserNetId, arg_SessionName, *SessionSettings);
		}
		else
		{
			return false;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));

		return false;
	}
}

bool UOnlineObject::JoinSession(TSharedPtr<const FUniqueNetId> arg_UserNetId, FName arg_SessionName, const FOnlineSessionSearchResult & arg_SearchResult)
{
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();

	if (OnlineSubsystemInterface)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();

		if (OnlineSessionInterface.IsValid() && arg_UserNetId.IsValid())
		{
			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			return OnlineSessionInterface->JoinSession(*arg_UserNetId, arg_SessionName, arg_SearchResult);
		}
	}

	return false;
}

void UOnlineObject::FindSessions(TSharedPtr<const FUniqueNetId> arg_UserNetId, bool arg_bIsLAN, bool arg_bIsPresence)
{
	// Get the OnlineSubsystem we want to work with
	const IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();

	if (OnlineSubsystemInterface)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();

		if (OnlineSessionInterface.IsValid() && arg_UserNetId.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = arg_bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;

			if (arg_bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, arg_bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			OnlineSessionInterface->FindSessions(*arg_UserNetId, SearchSettingsRef);
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

bool UOnlineObject::DestroySession(FName arg_SessionName)
{
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();

	if (OnlineSubsystemInterface)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();

		if (OnlineSessionInterface.IsValid() && arg_SessionName.IsValid())
		{
			OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			return OnlineSessionInterface->DestroySession(arg_SessionName);
		}
	}

	return false;
}

void UOnlineObject::OnDestroySessionComplete(FName arg_SessionName, bool arg_bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *arg_SessionName.ToString(), arg_bWasSuccessful));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();
	if (OnlineSubsystemInterface)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();

		if (OnlineSessionInterface.IsValid())
		{
			// Clear the Delegate
			OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}
}

void UOnlineObject::OnReadFriendsListComplete(int32 arg_LocalUserNum, bool arg_bWasSuccessful, const FString& arg_FriendsListName, const FString& arg_ErrorString)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnReadFriendsListComplete: %d"), arg_bWasSuccessful));

	if (arg_bWasSuccessful)
	{
		IOnlineFriendsPtr OnlineFriendInterface = Online::GetFriendsInterface();

		if (OnlineFriendInterface.IsValid())
		{
			OnlineFriendInterface->GetFriendsList(arg_LocalUserNum, arg_FriendsListName, FriendsList);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Failed to read friends: %s"), *arg_ErrorString));
	}
}

void UOnlineObject::OnSessionUserInviteAccepted(const bool arg_bWasSuccesful, const int32 arg_LocalUserNum, TSharedPtr<const FUniqueNetId> arg_NetId, const FOnlineSessionSearchResult& arg_SessionSearchResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnSessionUserInviteAccepted: %d"), arg_bWasSuccesful));

	if (arg_bWasSuccesful)
	{
		if (arg_SessionSearchResult.IsValid())
		{
			IOnlineSessionPtr SessionInt = IOnlineSubsystem::Get()->GetSessionInterface();
			SessionInt->JoinSession(arg_LocalUserNum, GameSessionName, arg_SessionSearchResult);
		}
	}
}

void UOnlineObject::OnCreateSessionComplete(FName arg_SessionName, bool arg_bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnCreateSessionComplete %d"), arg_bWasSuccessful));

	// Get the OnlineSubsystem so we can get the Session Interface
	const IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();
	if (OnlineSubsystemInterface)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();

		if (OnlineSessionInterface.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

			if (arg_bWasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
				
				OnlineSessionInterface->StartSession(arg_SessionName);
			}
		}
	}
}

void UOnlineObject::OnStartSessionComplete(FName arg_SessionName, bool arg_bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *arg_SessionName.ToString(), arg_bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	const IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();
	if (OnlineSubsystemInterface)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();
		if (OnlineSessionInterface.IsValid())
		{
			// Clear the delegate, since we are done with this call
			OnlineSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}
}

void UOnlineObject::OnFindSessionsComplete(bool arg_bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnFindSessionsComplete %d"), arg_bWasSuccessful));

	// Get OnlineSubsystem we want to work with
	const IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();
	if (OnlineSubsystemInterface)
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();
		if (OnlineSessionInterface.IsValid())
		{
			// Clear the Delegate handle, since we finished this call
			OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
			if (SessionSearch->SearchResults.Num() > 0)
			{
				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
				}
			}
		}
	}
}

void UOnlineObject::OnJoinSessionComplete(FName arg_SessionName, EOnJoinSessionCompleteResult::Type arg_Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *arg_SessionName.ToString(), static_cast<int32>(arg_Result)));

	// Get the OnlineSubsystem we want to work with
	const IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();
	if (OnlineSubsystemInterface)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();

		if (OnlineSessionInterface.IsValid())
		{
			// Clear the Delegate again
			OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		}
	}
}