#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatBasicEnemy.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class ACyberBeatMusicManager;

/**
 * An enemy that travels down one lane and reaches the hit zone (the track ring)
 * exactly at ArrivalSongTime. Its position is computed from the song clock every
 * frame, so it can never drift away from the music. If it is not killed it keeps
 * going and hits the core.
 */
UCLASS()
class ACyberBeatBasicEnemy : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatBasicEnemy();

    virtual void Tick(float DeltaTime) override;

    /** Called by the spawner right after spawning. */
    void Init(ACyberBeatMusicManager* InManager, const FVector& InCenter, float InLaneAngleDeg,
              float InSpawnRadius, float InHitRadius, float InArrivalSongTime, float InTravelTime);

    /** World yaw of the lane this enemy is in (0 = +X = W key, 90 = D, ...). */
    float GetLaneAngleDeg() const { return LaneAngleDeg; }

    /** Song time (seconds) at which this enemy should be hit. */
    float GetArrivalSongTime() const { return ArrivalSongTime; }

    /** Called by the character when the player kills it in the success zone. */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void Kill();

protected:
    /** Called when it reaches the core. Damage the core here once the core exists. */
    void ReachCore();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY()
    TObjectPtr<ACyberBeatMusicManager> Manager;

    /** How close to the center counts as "reached the core". */
    UPROPERTY(EditAnywhere, Category = "Tuning", meta = (ClampMin = "0.0", Units = "cm"))
    float CoreRadius = 100.f;

    UPROPERTY(EditAnywhere, Category = "Tuning", meta = (ClampMin = "0"))
    int32 Damage = 1;

    FVector Center = FVector::ZeroVector;
    float LaneAngleDeg = 0.f;
    float SpawnRadius = 1500.f;
    float HitRadius = 500.f;
    float ArrivalSongTime = 0.f;
    float TravelTime = 2.f;
};