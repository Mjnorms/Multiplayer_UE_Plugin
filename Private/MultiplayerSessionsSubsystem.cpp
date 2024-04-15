// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found Subsytem %s"), *Subsystem->GetSubsystemName().ToString())
			);
		}
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 numPublicConnections, FString matchType)
{
	if (!SessionInterface.IsValid()) return;

	GEngine->AddOnScreenDebugMessage(
		-1,
		15.f,
		FColor::Yellow,
		FString(TEXT("Creating Session"))
	);

	//check if session already exists, and destroy it if need be
	auto existingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	//Add delegate to interface delegate list, and store it in the handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	//set session settings
	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSettings->NumPublicConnections = numPublicConnections;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;

	SessionSettings->Set(FName("MatchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//create Session
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool created = SessionInterface->CreateSession(*(localPlayer->GetPreferredUniqueNetId()), NAME_GameSession, *SessionSettings);
	if (!created)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		MulitiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 numSearchResults)
{
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTECTED


void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	MulitiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName sessionName, bool bWasSuccessful)
{
}