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

USTRUCT(BlueprintType)
struct FCyberBeatSection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Section")
    FName Name = TEXT("Verse");

    /** Bar number where this section starts, as shown in FL Studio's playlist (bar 1 = start of the song). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Section", meta = (ClampMin = "1"))
    int32 StartBar = 1;

    /** 0 = no enemies, 1 = every beat, 2 = every other beat, 4 = one per bar (in 4/4). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Section", meta = (ClampMin = "0"))
    int32 EnemyEveryNBeats = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Event")
    int32 Lane = -1;

    
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


    UPROPERTY(EditAnywhere, Category = "Song Data", meta = (MultiLine = "true"))
    FString MusicEventText;


    UPROPERTY(Transient)
    TArray<FCyberBeatMusicEvent> MusicEvents;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing",
        meta = (ClampMin = "0.0"))
    float PerfectWindowMs = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing",
        meta = (ClampMin = "0.0"))
    float GoodWindowMs = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song", meta = (ClampMin = "1"))
    int32 BeatsPerBar = 4;

    /** Used only when MusicEventText is empty. List them in order of StartBar. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song")
    TArray<FCyberBeatSection> Sections;


    void ParseMusicEvents();
};
