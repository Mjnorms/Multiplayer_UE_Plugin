// Mjnorms -- 2024

#include "Menu.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UMenu::MenuSetup(int32 numConnections, FString typeOfMatch)
{
	m_numPublicConnections = numConnections;
	m_matchType = typeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessions -> Menu -> MenuSetup : GetWorld failed"));
		return;
	}
	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessions -> Menu -> MenuSetup : GetFirstPlayerController failed"));
		return;
	}

	FInputModeUIOnly inputModeData;
	inputModeData.SetWidgetToFocus(TakeWidget());
	inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	playerController->SetInputMode(inputModeData);
	playerController->SetShowMouseCursor(true);

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessions -> Menu -> MenuSetup : Get Game Instance failed"));
		return;
	}
	MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic	(this, &ThisClass::OnCreateSession);
		MultiplayerSessionSubsystem->MultiplayerOnFindSessionsComplete.AddUObject	(this, &ThisClass::OnFindSessions);
		MultiplayerSessionSubsystem->MultiplayerOnJoinSessionComplete.AddUObject	(this, &ThisClass::OnJoinSession);
		MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic	(this, &ThisClass::OnDestroySession);
		MultiplayerSessionSubsystem->MultiplayerOnStartSessionComplete.AddDynamic	(this, &ThisClass::OnStartSession);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTECTED
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTeardown();

	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Session Failed to be created!"))
		);
	}


	GEngine->AddOnScreenDebugMessage(
		-1,
		15.f,
		FColor::Yellow,
		FString(TEXT("Session Created Successfully!"))
	);

	//travel to the lobby and make it a listen server
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
	}
}

// TODO: Create a server list; for now, just connect to the first match type in 
// our steam region that matches this class's member variable m_matchType
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& sessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessions -> Menu -> OnFindSessions : MultiplayerSessionSubsystem does not exist"));
		return;
	}

	for (auto result : sessionResults)
	{
		FString result_matchType;
		result.Session.SessionSettings.Get(FName("MatchType"), result_matchType);
		if (result_matchType == m_matchType)
		{
			MultiplayerSessionSubsystem->JoinSession(result);
		}
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan,
		FString::Printf(TEXT("Join Result: %s"), result)
	);

	// Travel to IP Address
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		IOnlineSessionPtr sessionInterface = subsystem->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			FString address;
			sessionInterface->GetResolvedConnectString(NAME_GameSession, address);
			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UMenu::HostButtonClicked()
{
	if (MultiplayerSessionSubsystem)
	{
		//Create the session
		MultiplayerSessionSubsystem->CreateSession(m_numPublicConnections, m_matchType);
	}
}

void UMenu::JoinButtonClicked()
{
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSessions(SESSIONS_TO_FIND);
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessions -> Menu -> MenuTeardown : GetWorld failed"));
		return;
	}
	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessions -> Menu -> MenuTeardown : GetFirstPlayerController failed"));
		return;
	}
	FInputModeGameOnly InputModeData;
	playerController->SetInputMode(InputModeData);
	playerController->SetShowMouseCursor(false);
}
