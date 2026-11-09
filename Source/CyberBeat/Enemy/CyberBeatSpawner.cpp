#include "CyberBeatSpawner.h"
#include "CyberBeatBasicEnemy.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACyberBeatSpawner::ACyberBeatSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    EnemyClass = ACyberBeatBasicEnemy::StaticClass();
}

void ACyberBeatSpawner::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(SpawnTimer, this, &ACyberBeatSpawner::SpawnEnemy, SpawnInterval, true);
}

void ACyberBeatSpawner::EndPlay(const EEndPlayReason::Type Reason)
{
    GetWorldTimerManager().ClearTimer(SpawnTimer);
    Super::EndPlay(Reason);
}

void ACyberBeatSpawner::SpawnEnemy()
{
    if (!EnemyClass) return;

    float AngleDeg = FMath::FRandRange(0.f, 360.f);
    if (bSnapToLanes)
    {
        AngleDeg = 45.f * FMath::RandRange(0, 7);
    }

    const float AngleRad = FMath::DegreesToRadians(AngleDeg);
    const FVector Center = GetActorLocation();   // the spawner's position is treated as the core / track center
    const FVector SpawnLocation = Center + FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f) * SpawnRadius
                                  + FVector(0.f, 0.f, SpawnZ);

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (ACyberBeatBasicEnemy* Enemy = GetWorld()->SpawnActor<ACyberBeatBasicEnemy>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, Params))
    {
        Enemy->SetTargetLocation(Center + FVector(0.f, 0.f, SpawnZ));
    }
}