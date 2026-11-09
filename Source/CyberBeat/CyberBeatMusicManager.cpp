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

    if (!MusicTrack)
    {
        UE_LOG(LogTemp, Error, TEXT("No music assigned."));
        return;
    }

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
        this,ImmediateBoundary,EmptyCommandDelegate,NewClockHandle,BPM);


    FOnQuartzMetronomeEventBP BeatDelegate;
    BeatDelegate.BindUFunction(
        this,FName("HandleQuartzBeat"));

    NewClockHandle->SubscribeToQuantizationEvent(
        this,EQuartzCommandQuantization::Beat,BeatDelegate,NewClockHandle);

    NewClockHandle->StartClock(this, NewClockHandle);

    MusicComponent = NewObject<UAudioComponent>(this);

    if (MusicComponent)
    {
        MusicComponent->SetSound(MusicTrack);
        MusicComponent->bAutoActivate = false;
        MusicComponent->RegisterComponent();

        if (bStartMusicAutomatically)
        {
            FQuartzQuantizationBoundary MusicBoundary;
            MusicBoundary.Quantization = EQuartzCommandQuantization::Bar;
            MusicBoundary.Multiplier = 1.f;
            MusicBoundary.bFireOnClockStart = false;

            MusicComponent->PlayQuantized(
                this,NewClockHandle,MusicBoundary,EmptyCommandDelegate,0.f,0.f, 1.f,
                EAudioFaderCurve::Linear
            );
        }
    }

    UE_LOG(LogTemp,Warning,TEXT("Music Manager started. BPM: %.1f"),BPM );
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