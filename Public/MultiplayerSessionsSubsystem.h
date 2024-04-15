// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

//keep this down here
#include "MultiplayerSessionsSubsystem.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// New Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMulitiplayerOnCreateSessionComplete, bool, bWasSuccessful);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLass
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	

public:
	UMultiplayerSessionsSubsystem();

	// Session Functionality
	void CreateSession(int32 numPublicConnections, FString matchType);
	void FindSessions(int32 numSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	// class delegates
	FMulitiplayerOnCreateSessionComplete MulitiplayerOnCreateSessionComplete;

protected:
	// Internal Callbacks for the delegates
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type bWasSuccessful);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName sessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	// Delegates (Don't forget to bind callbacks)
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
};
