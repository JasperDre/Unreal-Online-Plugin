// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UnrealNetwork.h"
#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "UOnlineObject.generated.h"

/**
 * Unreal Online object to handle sessions, identity and friends.
 */
UCLASS()
class UOnlineObject : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Function to call create session.
	*
	* @param UserNetId: user that started the request.
	* @param SessionName: name of the session.
	* @param bIsLAN: is this is LAN Game?
	* @param bIsPresence: is the Session to create a presence session.
	* @param MaxNumPlayers: number of Maximum allowed players on this session.
	* @returns true if successful, false otherwise.
	*/
	bool CreateSession(TSharedPtr<const FUniqueNetId> arg_UserNetId, FName arg_SessionName, FName arg_Map, bool arg_bIsLAN, bool arg_bIsPresence, int32 arg_MaxNumPlayers);

	/**
	* Joins a session via a search result.
	*
	* @param UserNetId: user that started the request.
	* @param SessionName: name of session.
	* @param SearchResult: session to join.
	* @returns true if successful, false otherwise.
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> arg_UserNetId, FName arg_SessionName, const FOnlineSessionSearchResult& arg_SearchResult);

	/**
	* Find an online session.
	*
	* @param UserNetId: user that initiated the request.
	* @param bIsLAN: are we searching LAN matches.
	* @param bIsPresence: are we searching presence sessions.
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> arg_UserNetId, bool arg_bIsLAN, bool arg_bIsPresence);

	/**
	* Destroy an online session.
	*
	* @param SessionName: name of session.
	* @returns true if successful, false otherwise.
	*/
	bool DestroySession(FName arg_SessionName);

private:
	/**
	* Function fired when a session create request has completed.
	*
	* @param SessionName: the name of the session this callback is for.
	* @param bWasSuccessful: true if the async action completed without error, false if there was an error.
	*/
	void OnCreateSessionComplete(FName arg_SessionName, bool arg_bWasSuccessful);

	/**
	* Function fired when a session start request has completed.
	*
	* @param SessionName: the name of the session this callback is for.
	* @param bWasSuccessful: true if the async action completed without error, false if there was an error.
	*/
	void OnStartSessionComplete(FName arg_SessionName, bool arg_bWasSuccessful);

	/**
	* Delegate fired when a session search query has completed.
	*
	* @param bWasSuccessful: true if the async action completed without error, false if there was an error.
	*/
	void OnFindSessionsComplete(bool arg_bWasSuccessful);

	/**
	* Delegate fired when a session join request has completed.
	*
	* @param SessionName: the name of the session this callback is for.
	* @param bWasSuccessful: true if the async action completed without error, false if there was an error.
	*/
	void OnJoinSessionComplete(FName arg_SessionName, EOnJoinSessionCompleteResult::Type arg_Result);

	/**
	* Delegate fired when a destroying an online session has completed.
	*
	* @param SessionName: the name of the session this callback is for.
	* @param bWasSuccessful: true if the async action completed without error, false if there was an error.
	*/
	void OnDestroySessionComplete(FName arg_SessionName, bool arg_bWasSuccessful);

	/**
	 * Delegate used when reading friends list using query.
	 *
	 * @param LocalUserNum: the controller number of the associated user that made the request.
	 * @param bWasSuccessful: true if the async action completed without error, false if there was an error.
	 * @param FriendsListName: name of the friends list that was operated on.
	 * @param ErrorString: string representing the error condition.
	 */
	void OnReadFriendsListComplete(int32 arg_LocalUserNum, bool arg_bWasSuccessful, const FString& arg_FriendsListName, const FString& arg_ErrorString);

	/**
	 * Called when a user accepts a session invitation. Allows the game code a chance
	 * to clean up any existing state before accepting the invite. The invite must be
	 * accepted by calling JoinSession() after clean up has completed
	 *
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 * @param ControllerId the controller number of the accepting user
	 * @param UserId the user being invited
	 * @param InviteResult the search/settings for the session we're joining via invite
	 */
	void OnSessionUserInviteAccepted(const bool arg_bWasSuccesful, const int32 arg_LocalUserNum, TSharedPtr<const FUniqueNetId> arg_NetId, const FOnlineSessionSearchResult& arg_SessionSearchResult);

private:
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	TArray<TSharedRef<FOnlineFriend>> FriendsList;

	// Delegate called when session created
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	// Delegate called when session started 
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	// Delegate for searching for sessions
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	// Delegate for joining a session
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	// Delegate for destroying a session
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	// Delegate for reading friends list using query
	FOnReadFriendsListComplete OnReadFriendsListCompleteDelegate;

	// Delegate for when an invite is accepted (including rich presence)
	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;

	// Handles to registered delegates for creation
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	// Handles to registered delegates for starting
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	// Handle to registered delegate for searching a session
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	// Handle to registered delegate for joining a session
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	// Handle to registered delegate for destroying a session
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	// Handles to registered delegates for accepting an invite
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;
};
