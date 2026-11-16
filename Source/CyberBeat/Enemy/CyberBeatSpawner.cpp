#include "CyberBeatSpawner.h"
#include "CyberBeatBasicEnemy.h"
#include "CyberBeatMusicManager.h"
#include "CyberBeatTrack.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ACyberBeatSpawner::ACyberBeatSpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    EnemyClass = ACyberBeatBasicEnemy::StaticClass();
}

void ACyberBeatSpawner::BeginPlay()
{
    Super::BeginPlay();

    Manager = Cast<ACyberBeatMusicManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ACyberBeatMusicManager::StaticClass()));
    if (!Manager)
    {
        UE_LOG(LogTemp, Error, TEXT("CyberBeatSpawner: no music manager in the level, nothing will spawn"));
    }

    // Take the center and the hit radius from the track so everything agrees.
    const ACyberBeatTrack* Track = Cast<ACyberBeatTrack>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ACyberBeatTrack::StaticClass()));
    if (Track)
    {
        Center = Track->GetActorLocation();
        HitRadius = Track->GetTrackRadius();
    }
    else
    {
        Center = GetActorLocation();
        HitRadius = FallbackHitRadius;
        UE_LOG(LogTemp, Warning, TEXT("CyberBeatSpawner: no track in the level, using own location and FallbackHitRadius"));
    }
    Center.Z += SpawnZ;

    Stream.Initialize(LaneSeed);
    NextEventIndex = 0;
}

void ACyberBeatSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(Manager)) return;

    const TArray<FCyberBeatMusicEvent>& Events = Manager->GetMusicEvents();
    const float Now = Manager->GetSongTimeSeconds();

    while (NextEventIndex < Events.Num())
    {
        const FCyberBeatMusicEvent& Event = Events[NextEventIndex];

        if (Event.EventType == EMusicEventType::SectionStart)
        {
            ++NextEventIndex;
            continue;
        }

        // Events too early in the song cannot be spawned far enough out, so skip them.
        if (bSkipEarlyEvents && Event.TimeSeconds < TravelTime)
        {
            ++NextEventIndex;
            continue;
        }

        // Not time to spawn this one yet. Events are sorted, so nothing later is due either.
        if (Event.TimeSeconds - TravelTime > Now)
        {
            break;
        }

        const int32 Index = NextEventIndex++;
        if (Index % FMath::Max(BeatStride, 1) != 0)
        {
            continue;
        }

        SpawnForEvent(Event.TimeSeconds, Event.Lane);
    }
}

void ACyberBeatSpawner::SpawnForEvent(float EventTime, int32 EventLane)
{
    if (!EnemyClass) return;

    int32 Lane;
    if (EventLane >= 0)
    {
        Lane = EventLane % ACyberBeatTrack::NumLanes;   // lane from the chart
    }
    else if (LanePattern.Num() > 0)
    {
        Lane = FMath::Abs(LanePattern[PatternIndex % LanePattern.Num()]) % ACyberBeatTrack::NumLanes;
        ++PatternIndex;
    }
    else
    {
        Lane = Stream.RandRange(0, ACyberBeatTrack::NumLanes - 1);
    }

    const float AngleDeg = Lane * 45.f + LaneAngleOffset;
    const float Rad = FMath::DegreesToRadians(AngleDeg);

    const FVector SpawnLocation = Center + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0.f) * SpawnRadius;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (ACyberBeatBasicEnemy* Enemy = GetWorld()->SpawnActor<ACyberBeatBasicEnemy>(
            EnemyClass, SpawnLocation, FRotator(0.f, AngleDeg + 180.f, 0.f), Params))
    {
        Enemy->Init(Manager, Center, AngleDeg, SpawnRadius, HitRadius, EventTime, TravelTime);
    }
}