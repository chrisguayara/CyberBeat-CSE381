#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatSpawner.generated.h"

class ACyberBeatBasicEnemy;
class ACyberBeatMusicManager;
class ACyberBeatTrack;

UCLASS()
class ACyberBeatSpawner : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatSpawner();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    void SpawnForEvent(float EventTime, int32 EventLane);

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<ACyberBeatBasicEnemy> EnemyClass;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<ACyberBeatBasicEnemy> FlyingEnemyClass;

    /** Chance 0-1 that a spawn is a flying enemy. */
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FlyingChance = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<ACyberBeatBasicEnemy> SwitchingEnemyClass;

    /** Chance that a spawn is a switching enemy. */
    UPROPERTY(EditAnywhere, Category = "Spawning",meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SwitchingChance = 0.2f;

    /** How far from the center enemies appear. Must be larger than the track radius. */
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0.0", Units = "cm"))
    float SpawnRadius = 1500.f;

    /** Seconds an enemy takes to travel from the spawn point to the hit zone. */
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0.2", Units = "s"))
    float TravelTime = 2.f;

    /** 1 = an enemy on every beat, 2 = every other beat, and so on. */
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "1"))
    int32 BeatStride = 2;

    /** Skip events earlier than TravelTime into the song, which could not spawn far enough out. */
    UPROPERTY(EditAnywhere, Category = "Spawning")
    bool bSkipEarlyEvents = true;

    /** Same seed = same lane pattern every run. */
    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 LaneSeed = 1234;

    /** Height above the track center to spawn and travel at. */
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnZ = 100.f;

    /** Used only if no track actor exists in the level. */
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0.0", Units = "cm"))
    float FallbackHitRadius = 500.f;

    /** Added to every lane angle. Leave at 0 unless the character's LaneYawOffset is not 0. */
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float LaneAngleOffset = 0.f;

    UPROPERTY()
    TObjectPtr<ACyberBeatMusicManager> Manager;

    FVector Center = FVector::ZeroVector;
    float HitRadius = 500.f;
    int32 NextEventIndex = 0;
    FRandomStream Stream;
    FRandomStream FlyStream;
    FRandomStream SwitchStream;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TArray<int32> LanePattern;

    int32 PatternIndex = 0;
};