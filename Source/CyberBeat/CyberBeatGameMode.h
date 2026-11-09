// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CyberBeatGameMode.generated.h"

/**
 *  Simple Game Mode for a top-down perspective game
 *  Sets the default gameplay framework classes
 *  Check the Blueprint derived class for the set values
 */
UCLASS()
class CYBERBEAT_API ACyberBeatGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	ACyberBeatGameMode();


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Progression")
    TArray<FName> Levels;
  
    UPROPERTY(BlueprintReadOnly, Category = "Level Progression")
    int32 CurrentLevel = 0;


    UFUNCTION(BlueprintCallable, Category = "Level Progression")
    void LoadLevel(int32 LevelIndex);

  
    UFUNCTION(BlueprintCallable, Category = "Level Progression")
    void LoadNextLevel();


    UFUNCTION(BlueprintCallable, Category = "Level Progression")
    void ReturnToMainMenu();
};



