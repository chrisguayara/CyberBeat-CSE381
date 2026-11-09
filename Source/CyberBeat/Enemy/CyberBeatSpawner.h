#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatSpawner.generated.h"

class ACyberBeatBasicEnemy;


UCLASS()
class ACyberBeatSpawner : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatSpawner();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    void SpawnEnemy();

    UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<ACyberBeatBasicEnemy> EnemyClass;

    /** Distance from the center. Make this larger than the track radius. */
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0.0", Units = "cm"))
    float SpawnRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0.1", Units = "s"))
    float SpawnInterval = 3.f;

    /** If true, snap to one of 8 lane angles instead of any angle. Off for now. */
    UPROPERTY(EditAnywhere, Category = "Spawning")
    bool bSnapToLanes = false;

    /** Height to spawn at, so enemies sit on the track surface. */
    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnZ = 100.f;

    FTimerHandle SpawnTimer;
};