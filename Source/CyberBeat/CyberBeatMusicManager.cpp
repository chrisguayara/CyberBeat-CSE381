#include "CyberBeatMusicManager.h"

#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

#include "Quartz/QuartzSubsystem.h"
#include "Quartz/AudioMixerClockHandle.h"

ACyberBeatMusicManager::ACyberBeatMusicManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACyberBeatMusicManager::BeginPlay()
{
    Super::BeginPlay();

    if (!SongData || !SongData->MusicTrack) return;

    //if (!MusicTrack)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("No music assigned."));
    //    return;
    //}

    UQuartzSubsystem* QuartzSubsystem =GetWorld()->GetSubsystem<UQuartzSubsystem>();

    if (!QuartzSubsystem)
    {
        return;
    }

    FQuartzClockSettings ClockSettings;

    UQuartzClockHandle* NewClockHandle =
        QuartzSubsystem->CreateNewClock(this,FName("CyberBeatClock"), ClockSettings, true,true );

    if (!NewClockHandle)
    {
        return;
    }

    ClockHandle = NewClockHandle;

    FQuartzQuantizationBoundary ImmediateBoundary;
    FOnQuartzCommandEventBP EmptyCommandDelegate;

    NewClockHandle->SetBeatsPerMinute(
        this,ImmediateBoundary,EmptyCommandDelegate,NewClockHandle, SongData->BPM);


    FOnQuartzMetronomeEventBP BeatDelegate;
    BeatDelegate.BindUFunction(
        this,FName("HandleQuartzBeat"));

    NewClockHandle->SubscribeToQuantizationEvent(
        this,EQuartzCommandQuantization::Beat,BeatDelegate,NewClockHandle);

    NewClockHandle->StartClock(this, NewClockHandle);

    MusicComponent = NewObject<UAudioComponent>(this);

    if (MusicComponent)
    {
        MusicComponent->SetSound(SongData->MusicTrack);
        
        MusicComponent->bAutoActivate = false;
        MusicComponent->RegisterComponent();

        if (bStartMusicAutomatically)
        {
            MusicStartTime = GetWorld()->GetTimeSeconds();
            MusicComponent->Play();
        }
    }

    UE_LOG(LogTemp,Warning,TEXT("Music Manager started. BPM: %.1f"), SongData->BPM);
}

void ACyberBeatMusicManager::HandleQuartzBeat(
    FName ClockName,EQuartzCommandQuantization QuantizationType,
    int32 NumBars,int32 Beat,float BeatFraction)
{
    CurrentBeat++;
    UE_LOG(LogTemp, Warning,
        TEXT("Beat count: %d"),
        CurrentBeat);
    OnBeat.Broadcast(CurrentBeat);
}

void ACyberBeatMusicManager::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (MusicComponent)
    {
        MusicComponent->Stop();
    }

    if (ClockHandle)
    {
        UQuartzClockHandle* ClockHandleRaw = ClockHandle.Get();
        ClockHandleRaw->StopClock(this, true, ClockHandleRaw);
    }

    Super::EndPlay(EndPlayReason);
}


float ACyberBeatMusicManager::GetSongTimeSeconds() const
{
    if (!MusicComponent || !MusicComponent->IsPlaying() || !GetWorld())
    {
        return 0.0f;
    }

    const double ElapsedTime =
        GetWorld()->GetTimeSeconds() - MusicStartTime;

    return static_cast<float>(
        ElapsedTime - SongData->AudioStartOffsetSeconds
        );
}

EBeatTimingResult ACyberBeatMusicManager::JudgeInput(
    float& TimingOffset
) const
{
    TimingOffset = 0.0f;

    if (!SongData || !MusicComponent || !MusicComponent->IsPlaying())
    {
        return EBeatTimingResult::Miss;
    }

    const float CurrentTime = GetSongTimeSeconds();

    float BestDifference = TNumericLimits<float>::Max();

    bool bFoundTimingEvent = false;

    for (const FCyberBeatMusicEvent& Event : SongData->MusicEvents)
    {
        if (Event.EventType == EMusicEventType::SectionStart)
        {
            continue;
        }

        const float Difference = CurrentTime - Event.TimeSeconds;

        if (FMath::Abs(Difference) < FMath::Abs(BestDifference))
        {
            BestDifference = Difference;
            bFoundTimingEvent = true;
        }
    }

    if (!bFoundTimingEvent)
    {
        return EBeatTimingResult::Miss;
    }


    TimingOffset = BestDifference * 1000.0f;

    const float AbsoluteDifference = FMath::Abs(TimingOffset);

    if (AbsoluteDifference <= SongData->PerfectWindowMs)
    {
        return EBeatTimingResult::Perfect;
    }

    if (AbsoluteDifference <= SongData->GoodWindowMs)
    {
        return EBeatTimingResult::Good;
    }

    return EBeatTimingResult::Miss;
}