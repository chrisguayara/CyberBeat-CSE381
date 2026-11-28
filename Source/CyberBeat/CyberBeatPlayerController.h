// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CyberBeatPlayerController.generated.h"

class UInputAction;
class UCyberBeatPauseMenu;
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UPathFollowingComponent;
class UCyberBeatLevelSelectWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Player controller for a top-down perspective game.
 *  Implements point and click based controls
 */
UCLASS()
class ACyberBeatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACyberBeatPlayerController();

protected:


	// UI initialization
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;


	// Level selection widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCyberBeatLevelSelectWidget> LevelSelectWidgetClass;

	UPROPERTY()
	TObjectPtr<UCyberBeatLevelSelectWidget> LevelSelectWidget;



	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UCyberBeatPauseMenu> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<UCyberBeatPauseMenu> PauseMenu;

	void TogglePause();



};


