// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CyberBeatSongData.h"
#include "GameFramework/PlayerState.h"
#include "CyberBeatPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnScoreUpdated,
    int32, NewScore,
    int32, NewCombo,
    int32, NewBestCombo
);
/**
 * 
 */
UCLASS()
class CYBERBEAT_API ACyberBeatPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    ACyberBeatPlayerState();

    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 TotalScore = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 Combo = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 BestCombo = 0;

    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreUpdated OnScoreUpdated;

    void RegisterHit(EBeatTimingResult Result);

    void ResetScore();
	
};
