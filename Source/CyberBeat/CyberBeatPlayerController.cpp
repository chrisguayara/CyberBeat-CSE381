// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberBeatPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CyberBeatCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "CyberBeat.h"
#include "CyberBeatLevelSelectWidget.h"
#include "CyberBeatPauseMenu.h"
#include "CyberBeatMusicManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ACyberBeatPlayerController::ACyberBeatPlayerController()
{
	
	// configure the controller
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
}

void ACyberBeatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Check if this is the main menu
	if (GetWorld()->GetMapName().Contains(TEXT("MainMenu")))
	{
		// Show level selection menu
		if ( LevelSelectWidgetClass)
		{
			LevelSelectWidget = CreateWidget<UCyberBeatLevelSelectWidget>(
				this,
				LevelSelectWidgetClass
			);

			if (LevelSelectWidget)
			{
				LevelSelectWidget->AddToViewport();

				bShowMouseCursor = true;

				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LevelSelectWidget->TakeWidget());

				SetInputMode(InputMode);
			}
		}
	}
	else
	{
		// Restore mouse controls
		bShowMouseCursor = false;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}

	
}

void ACyberBeatPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Only set up input on local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{

			if (PauseAction)
			{
				EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ACyberBeatPlayerController::TogglePause);
			}

		}
		else
		{
			UE_LOG(LogCyberBeat, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
		}
	}
}



void ACyberBeatPlayerController::TogglePause()
{

	if (GetWorld()->GetMapName().Contains(TEXT("MainMenu"))) return;

	
	if (PauseMenu && PauseMenu->IsInViewport())
	{
		PauseMenu->Resume();
		return;
	}

	if (const ACyberBeatCharacter* Char = Cast<ACyberBeatCharacter>(GetPawn()))
	{
		if (!Char->CanPause()) return;
	}


	for (TActorIterator<ACyberBeatMusicManager> It(GetWorld()); It; ++It)
	{
		if (It->IsFailed()) return;
	}

	if (!PauseMenuClass) return;

	for (TActorIterator<ACyberBeatMusicManager> It(GetWorld()); It; ++It)
	{
		It->PauseMusic();
	}
	UGameplayStatics::SetGamePaused(this, true);

	PauseMenu = CreateWidget<UCyberBeatPauseMenu>(this, PauseMenuClass);
	PauseMenu->AddToViewport();

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PauseMenu->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

