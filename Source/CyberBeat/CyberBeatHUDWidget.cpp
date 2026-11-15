// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatHUDWidget.h"
#include "CyberBeatPlayerState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "CyberBeatMusicManager.h"


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

    AActor* FoundActor = UGameplayStatics::GetActorOfClass(
        GetWorld(),ACyberBeatMusicManager::StaticClass());

    MusicManager = Cast<ACyberBeatMusicManager>(FoundActor);

    
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


    AActor* FoundActor = UGameplayStatics::GetActorOfClass(
        GetWorld(), ACyberBeatMusicManager::StaticClass());

    MusicManager = Cast<ACyberBeatMusicManager>(FoundActor);

    if (!MusicManager)
    {
        UE_LOG(LogTemp, Warning,TEXT("Can't find Music Manager."));
    }


}


void UCyberBeatHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!MusicManager || !BeatIndicator)
    {
        return;
    }

    float TimeUntilNextEvent = 0.0f;
    float TimeSinceLastEvent = 0.0f;
    float EventInterval = 0.5f;

    bool bNextIsAccent = false;
    bool bLastWasAccent = false;

    bool bHasEvents = MusicManager->GetMusicIndicatorData(TimeUntilNextEvent,
        TimeSinceLastEvent, EventInterval, bNextIsAccent, bLastWasAccent );

    if (!bHasEvents) return;
    
    float Progress = FMath::Clamp(
        1.0f - (TimeUntilNextEvent / EventInterval),0.0f,1.0f);

    float Scale = FMath::Lerp(0.2f, 1.0f, Progress);

    InnerBeatCircle->SetRenderScale( FVector2D(Scale, Scale));

    FLinearColor CircleColor = bNextIsAccent
        ? FLinearColor(1.0f, 0.65f, 0.0f, 1.0f)
        : FLinearColor(0.0f, 0.8f, 1.0f, 1.0f);

    InnerBeatCircle->SetColorAndOpacity(CircleColor);
}
