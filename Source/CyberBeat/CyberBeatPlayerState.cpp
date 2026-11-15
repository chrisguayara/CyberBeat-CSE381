// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatPlayerState.h"

ACyberBeatPlayerState::ACyberBeatPlayerState()
{
    TotalScore = 0;
    Combo = 0;
    BestCombo = 0;
}

void ACyberBeatPlayerState::RegisterHit(EBeatTimingResult Result)
{
    switch (Result)
    {
    case EBeatTimingResult::Perfect:
        TotalScore += 1000;
        Combo++;
        break;

    case EBeatTimingResult::Good:
        TotalScore += 500;
        Combo++;
        break;

    case EBeatTimingResult::Miss:
        Combo = 0;
        break;
    }

    BestCombo = FMath::Max(BestCombo, Combo);

    OnScoreUpdated.Broadcast(TotalScore, Combo, BestCombo);

    UE_LOG(LogTemp, Warning,TEXT("Total Score: %d, Combo: %d, Best Combo: %d"),
        TotalScore, Combo, BestCombo);
}