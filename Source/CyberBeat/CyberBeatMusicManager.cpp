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

    
    SongData->ParseMusicEvents();

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

bool ACyberBeatMusicManager::GetMusicIndicatorData(
	float& TimeUntilNextEvent, float& TimeSinceLastEvent, float& EventInterval,
	bool& bNextIsAccent, bool& bLastWasAccent
) const
{

    TimeUntilNextEvent = 0.0f;
    TimeSinceLastEvent = -1.0f;
    EventInterval = 1.0f;
    bNextIsAccent = false;
    bLastWasAccent = false;

    if (!SongData || SongData->MusicEvents.Num() == 0)
    {
        return false;
    }

    const float CurrentSongTime = GetSongTimeSeconds();

    const FCyberBeatMusicEvent* PreviousEvent = nullptr;
    const FCyberBeatMusicEvent* NextEvent = nullptr;

    // Find the previous and next playable music events.
    for (const FCyberBeatMusicEvent& Event : SongData->MusicEvents)
    {
        if (Event.EventType == EMusicEventType::SectionStart)
        {
            continue;
        }

        if (Event.TimeSeconds <= CurrentSongTime)
        {
            PreviousEvent = &Event;
        }
        else
        {
            NextEvent = &Event;
            break;
        }
    }

    if (!NextEvent) return false;

    TimeUntilNextEvent = NextEvent->TimeSeconds - CurrentSongTime;
    bNextIsAccent = (NextEvent->EventType == EMusicEventType::Accent);

    if (PreviousEvent)
    {
        TimeSinceLastEvent = CurrentSongTime - PreviousEvent->TimeSeconds;
        EventInterval = NextEvent->TimeSeconds - PreviousEvent->TimeSeconds;
        bLastWasAccent = (PreviousEvent->EventType == EMusicEventType::Accent);
    }
    else
    {
    
        EventInterval = FMath::Max(NextEvent->TimeSeconds, 0.001f);
    }

    EventInterval = FMath::Max(EventInterval, 0.001f);

    return true;
}


const TArray<FCyberBeatMusicEvent>& ACyberBeatMusicManager::GetMusicEvents() const
{
    static const TArray<FCyberBeatMusicEvent> Empty;
    return SongData ? SongData->MusicEvents : Empty;
}

EBeatTimingResult ACyberBeatMusicManager::JudgeAgainstTime(float TargetSongTime, float& TimingOffsetMs) const
{
    TimingOffsetMs = 0.f;
    if (!SongData || !MusicComponent || !MusicComponent->IsPlaying()) return EBeatTimingResult::Miss;

    TimingOffsetMs = (GetSongTimeSeconds() - TargetSongTime) * 1000.f;
    const float Abs = FMath::Abs(TimingOffsetMs);
    if (Abs <= SongData->PerfectWindowMs) return EBeatTimingResult::Perfect;
    if (Abs <= SongData->GoodWindowMs)    return EBeatTimingResult::Good;
    return EBeatTimingResult::Miss;
}