// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberBeatGameMode.h"
#include "Kismet/GameplayStatics.h"

ACyberBeatGameMode::ACyberBeatGameMode()
{
	// stub
}

void ACyberBeatGameMode::LoadLevel(int32 LevelIndex)
{ 
    if (Levels.IsValidIndex(LevelIndex))
    {
        CurrentLevel = LevelIndex;

        UGameplayStatics::OpenLevel(
            this,
            Levels[LevelIndex]);
    }
}

void ACyberBeatGameMode::LoadNextLevel()
{
    int32 NextIndex = CurrentLevel + 1;

    if (Levels.IsValidIndex(NextIndex))
    {
        LoadLevel(NextIndex);
    }
  
}

void ACyberBeatGameMode::ReturnToMainMenu()
{
    UGameplayStatics::OpenLevel(
        this,
        FName("MainMenu")
    );
}