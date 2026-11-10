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

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    int32 GetCurrentBeat() const { return CurrentBeat; }

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    float GetBPM() const { return SongData ? SongData->BPM : 0.0f; }

    UFUNCTION(BlueprintPure, Category = "Music Manager")
    float GetSongTimeSeconds() const;

    UFUNCTION(BlueprintCallable, Category = "Music Manager")
    EBeatTimingResult JudgeInput(float& TimingOffset) const;


protected:

    UPROPERTY(EditAnywhere, Category = "Music")
    TObjectPtr<UCyberBeatSongData> SongData;

    UPROPERTY(EditAnywhere, Category = "Music")
    bool bStartMusicAutomatically = true;

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
};