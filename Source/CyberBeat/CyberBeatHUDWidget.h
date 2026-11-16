// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "CyberBeatCore.h"
#include "CyberBeatSongData.h"
#include "CyberBeatHUDWidget.generated.h"


class UTextBlock;
class ACyberBeatPlayerState;
class UImage;
class ACyberBeatMusicManager;
class UProgressBar;


/**
 * 
 */
UCLASS()
class CYBERBEAT_API UCyberBeatHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

    virtual void NativeConstruct() override;
    virtual void NativeTick( const FGeometry& MyGeometry, float InDeltaTime ) override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> BeatIndicator;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> InnerBeatCircle;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> SongProgressBar;

    UPROPERTY()
    TObjectPtr<ACyberBeatMusicManager> MusicManager;


    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> CoreHealthBar;

    UPROPERTY()
    TObjectPtr<ACyberBeatCore> CoreActor;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ScoreText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ComboText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> BestComboText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TimingFeedbackText;

    UFUNCTION()
    void UpdateScoreDisplay(int32 NewScore, int32 NewCombo, int32 NewBestCombo );







public:

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowTiming(EBeatTimingResult Result);

    UFUNCTION()
    void HandleCoreHealthChanged(int32 Health, int32 MaxHealth);
};
