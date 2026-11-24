#include "CyberBeatSpawner.h"
#include "CyberBeatBasicEnemy.h"
#include "CyberBeatMusicManager.h"
#include "CyberBeatTrack.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

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
    FlyStream.Initialize(LaneSeed + 1);
    SwitchStream.Initialize(LaneSeed + 2);

    PatternIndex = 0;
    NextEventIndex = 0;
    SpawnCount = 0;          // NEW
    PowerupsSpawned = 0;     // NEW
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

        if (bSkipEarlyEvents && Event.TimeSeconds < TravelTime)
        {
            ++NextEventIndex;
            continue;
        }

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

    UpdateEnemyGlow();
}

void ACyberBeatSpawner::SpawnForEvent(float EventTime, int32 EventLane)
{
    if (!EnemyClass) return;

    int32 Lane;
    if (EventLane >= 0)
    {
        Lane = EventLane % ACyberBeatTrack::NumLanes;
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

    TSubclassOf<ACyberBeatBasicEnemy> ClassToSpawn = EnemyClass;
    const float FlyProbability = FlyingEnemyClass ? FMath::Clamp(FlyingChance, 0.f, 1.f) : 0.f;
    const float SwitchProbability = SwitchingEnemyClass ? FMath::Clamp(SwitchingChance, 0.f, 1.f - FlyProbability) : 0.f;
    const float Roll = SwitchStream.FRand();

    if (FlyingEnemyClass && Roll < FlyProbability)
    {
        ClassToSpawn = FlyingEnemyClass;
    }
    else if (SwitchingEnemyClass && Roll < FlyProbability + SwitchProbability)
    {
        ClassToSpawn = SwitchingEnemyClass;
    }

    // NEW: decide if this spawn is an exploding variant
    ++SpawnCount;
    const bool bMakePowerup = PowerupsSpawned < MaxPowerups
                           && SpawnCount % FMath::Max(PowerupInterval, 2) == 0;

    const float AngleDeg = Lane * 45.f + LaneAngleOffset;
    const float Rad = FMath::DegreesToRadians(AngleDeg);

    const FVector SpawnLocation = Center + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0.f) * SpawnRadius;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (ACyberBeatBasicEnemy* Enemy = GetWorld()->SpawnActor<ACyberBeatBasicEnemy>(
        ClassToSpawn, SpawnLocation, FRotator(0.f, AngleDeg + 180.f, 0.f), Params))
    {
        Enemy->Init(Manager, Center, AngleDeg, SpawnRadius, HitRadius, EventTime, TravelTime);

        // NEW
        if (bMakePowerup)
        {
            Enemy->MakePowerup();
            ++PowerupsSpawned;
        }
    }
}

void ACyberBeatSpawner::UpdateEnemyGlow()
{
    const float Now = Manager->GetSongTimeSeconds();

    TArray<ACyberBeatBasicEnemy*> Live;
    for (TActorIterator<ACyberBeatBasicEnemy> It(GetWorld()); It; ++It)
    {
        if (It->GetArrivalSongTime() >= Now - GlowGraceSeconds) { Live.Add(*It); }
        else { It->SetGlowTarget(0.f); }
    }

    Live.Sort([](const ACyberBeatBasicEnemy& A, const ACyberBeatBasicEnemy& B)
    {
        return A.GetArrivalSongTime() < B.GetArrivalSongTime();
    });

    for (int32 i = 0; i < Live.Num(); ++i)
    {
        Live[i]->SetGlowTarget(GlowLevels.IsValidIndex(i) ? GlowLevels[i] : 0.f);
    }
}