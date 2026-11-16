#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/QuartzQuantizationUtilities.h"
#include "CyberBeatSongData.h"
#include "CyberBeatMusicManager.generated.h"

class USoundBase;
class UAudioComponent;
class UQuartzClockHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnCyberBeat,
    int32,
    BeatNumber
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCyberBeatSongFinished);

UCLASS()
class CYBERBEAT_API ACyberBeatMusicManager : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatMusicManager();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(BlueprintAssignable, Category = "Music Manager")
    FOnCyberBeat OnBeat;

    UPROPERTY(BlueprintAssignable, Category = "Music Manager")
    FOnCyberBeatSongFinished OnSongFinished;

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    int32 GetCurrentBeat() const { return CurrentBeat; }

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    float GetBPM() const { return SongData ? SongData->BPM : 0.0f; }

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    float GetSongTimeSeconds() const;

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    float GetSongDurationSeconds() const;

    UFUNCTION(BlueprintCallable, Category = "Music Manager")
    EBeatTimingResult JudgeInput(float& TimingOffset) const;

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    bool GetMusicIndicatorData(
        float& TimeUntilNextEvent,float& TimeSinceLastEvent,float& EventInterval,
        bool& bNextIsAccent,bool& bLastWasAccent ) const;

    const TArray<FCyberBeatMusicEvent>& GetMusicEvents() const;
    EBeatTimingResult JudgeAgainstTime(float TargetSongTime, float& TimingOffsetMs) const;


    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Music Manager")
    void StartFailSequence();

    bool IsFailed() const { return bFailed; }
protected:

    UPROPERTY(EditAnywhere, Category = "Music")
    TObjectPtr<UCyberBeatSongData> SongData;

    UPROPERTY(EditAnywhere, Category = "Music")
    bool bStartMusicAutomatically = true;

    UPROPERTY(EditAnywhere, Category = "Fail", meta = (ClampMin = "0.2", Units = "s"))
    float FailSlowdownSeconds = 2.5f;

    /** Pitch/speed the song winds down to before stopping. */
    UPROPERTY(EditAnywhere, Category = "Fail", meta = (ClampMin = "0.1", ClampMax = "1.0"))
    float FailMinPitch = 0.15f;

private:
    UFUNCTION()
    void HandleQuartzBeat(
        FName ClockName,
        EQuartzCommandQuantization QuantizationType,
        int32 NumBars,int32 Beat,float BeatFraction);

    UPROPERTY()
    TObjectPtr<UAudioComponent> MusicComponent;

    UPROPERTY()
    TObjectPtr<UQuartzClockHandle> ClockHandle;

    int32 CurrentBeat = 0;

    double MusicStartTime = 0.0;
    
    bool bFailed = false;
    float FailElapsed = 0.f;
    float FrozenSongTime = 0.f;

    UFUNCTION()
    void HandleAudioFinished();

    bool bSongFinished = false;
    bool bEnding = false;
};