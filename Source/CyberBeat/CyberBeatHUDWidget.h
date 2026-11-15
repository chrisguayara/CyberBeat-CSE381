// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CyberBeatSongData.h"
#include "CyberBeatHUDWidget.generated.h"


class UTextBlock;
class ACyberBeatPlayerState;
/**
 * 
 */
UCLASS()
class CYBERBEAT_API UCyberBeatHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

    virtual void NativeConstruct() override;

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
};
