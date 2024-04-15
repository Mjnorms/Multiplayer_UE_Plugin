// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 numConnections = 4, FString typeOfMatch = FString(TEXT("FFA")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//callbacks
	UFUNCTION()
	void OnCreateSessionComplete( bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTeardown();

	// subsystem designed to handle all online func
	class UMultiplayerSessionsSubsystem* MultiplayerSessionSubsystem;

	//variables
	int32 numPublicConnections{ 4 };
	FString matchType{TEXT("FFA")};

	FMulitiplayerOnCreateSessionComplete MulitiplayerOnCreateSessionComplete;
};
