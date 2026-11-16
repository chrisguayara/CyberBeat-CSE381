// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatGameOverWidget.h"
#include "Components/TextBlock.h"


void UCyberBeatGameOverWidget::SetFinalScore(int32 Score)
{
    if (FinalScoreText)
    {
        FinalScoreText->SetText(FText::FromString(
            FString::Printf(TEXT("FINAL SCORE: %d"), Score)));
    }
}

