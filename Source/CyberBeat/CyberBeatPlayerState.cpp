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
        PerfectCount++;
        break;

    case EBeatTimingResult::Good:
        TotalScore += 500;
        Combo++;
        GoodCount++;
        break;

    case EBeatTimingResult::Miss:
        Combo = 0;
        MissCount++;
        break;
    }

    BestCombo = FMath::Max(BestCombo, Combo);

    OnScoreUpdated.Broadcast(TotalScore, Combo, BestCombo);

    UE_LOG(LogTemp, Warning,TEXT("Total Score: %d, Combo: %d, Best Combo: %d"),
        TotalScore, Combo, BestCombo);
}

float ACyberBeatPlayerState::GetAccuracyPercent() const
{
    const int32 Total = PerfectCount + GoodCount + MissCount;
    return Total > 0 ? 100.f * (PerfectCount + GoodCount) / Total : 0.f;
}

void ACyberBeatPlayerState::ResetScore()
{
    TotalScore = 0;
    Combo = 0;
    BestCombo = 0;
    PerfectCount = 0;
    GoodCount = 0;
    MissCount = 0;

    OnScoreUpdated.Broadcast(TotalScore, Combo, BestCombo);
}