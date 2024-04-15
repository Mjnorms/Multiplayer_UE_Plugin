// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UMenu::MenuSetup(int32 numConnections, FString typeOfMatch)
{
	numPublicConnections = numConnections;
	matchType = typeOfMatch;
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

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& sessionResults, bool bWasSuccessful)
{
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
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
		MultiplayerSessionSubsystem->CreateSession(numPublicConnections, matchType);
	}
}

void UMenu::JoinButtonClicked()
{

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Join Button Clicked"))
		);
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
