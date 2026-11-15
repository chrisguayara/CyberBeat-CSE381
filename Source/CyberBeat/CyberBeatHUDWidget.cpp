// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatHUDWidget.h"
#include "CyberBeatPlayerState.h"
#include "Components/TextBlock.h"


void UCyberBeatHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
    ACyberBeatPlayerState* ScoreState =GetOwningPlayerState<ACyberBeatPlayerState>();

    if (ScoreState)
    {
        ScoreState->OnScoreUpdated.AddDynamic(
            this,&UCyberBeatHUDWidget::UpdateScoreDisplay );

        UpdateScoreDisplay( ScoreState->TotalScore, ScoreState->Combo,  ScoreState->BestCombo );
    }

    if (TimingFeedbackText)
    {
        TimingFeedbackText->SetText(FText::GetEmpty());
    }
}

void UCyberBeatHUDWidget::UpdateScoreDisplay(int32 NewScore, int32 NewCombo, int32 NewBestCombo)
{
	if (ScoreText)
	{
        ScoreText->SetText(FText::FromString(FString::Printf(TEXT("SCORE: %d"), NewScore) ));
	}
	if (ComboText)
	{
		ComboText->SetText(FText::FromString(FString::Printf(TEXT("COMBO: x%d"), NewCombo) ));
	}
	if (BestComboText)
	{
		BestComboText->SetText(FText::FromString(FString::Printf(TEXT("BEST COMBO: %d"), NewBestCombo) ));
	}
}

void UCyberBeatHUDWidget::ShowTiming(EBeatTimingResult Result)
{
	if (TimingFeedbackText)
	{
        switch (Result)
        {
        case EBeatTimingResult::Perfect:
            TimingFeedbackText->SetText(FText::FromString(TEXT("PERFECT!")));
            TimingFeedbackText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
            break;

        case EBeatTimingResult::Good:
            TimingFeedbackText->SetText(FText::FromString(TEXT("GOOD!")));
            TimingFeedbackText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
            break;

        case EBeatTimingResult::Miss:
            TimingFeedbackText->SetText(FText::FromString(TEXT("MISS!")));
            TimingFeedbackText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
            break;
        }
	}
}