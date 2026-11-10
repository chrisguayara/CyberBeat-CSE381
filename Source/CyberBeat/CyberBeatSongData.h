// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CyberBeatSongData.generated.h"

/**
 * 
 */
class USoundBase;

UENUM(BlueprintType)
enum class EMusicEventType : uint8
{
    Beat UMETA(DisplayName = "Beat"),
    Accent UMETA(DisplayName = "Accent"),
    SectionStart UMETA(DisplayName = "Section Start")
};

UENUM(BlueprintType)
enum class EBeatTimingResult : uint8
{
    Perfect UMETA(DisplayName = "Perfect"),
    Good    UMETA(DisplayName = "Good"),
    Miss    UMETA(DisplayName = "Miss")
};



USTRUCT(BlueprintType)
struct FCyberBeatMusicEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Event",
        meta = (ClampMin = "0.0"))
    float TimeSeconds = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Event")
    EMusicEventType EventType = EMusicEventType::Beat;

};


UCLASS(BlueprintType)
class CYBERBEAT_API UCyberBeatSongData : public UDataAsset
{
	GENERATED_BODY()
	
public:
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song")
    TObjectPtr<USoundBase> MusicTrack;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song",
        meta = (ClampMin = "1.0"))
    float BPM = 120.0f;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song")
    float AudioStartOffsetSeconds = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song")
    TArray<FCyberBeatMusicEvent> MusicEvents;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing",
        meta = (ClampMin = "0.0"))
    float PerfectWindowMs = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing",
        meta = (ClampMin = "0.0"))
    float GoodWindowMs = 150.0f;
};
