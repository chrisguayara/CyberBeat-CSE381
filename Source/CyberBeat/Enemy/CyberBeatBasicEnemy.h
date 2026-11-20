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
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Called by the spawner right after spawning. */
    void Init(ACyberBeatMusicManager* InManager, const FVector& InCenter, float InLaneAngleDeg,
              float InSpawnRadius, float InHitRadius, float InArrivalSongTime, float InTravelTime);

    /** World yaw of the lane this enemy is in (0 = +X = W key, 90 = D, ...). */
    float GetLaneAngleDeg() const { return LaneAngleDeg; }

    /** Song time (seconds) at which this enemy should be hit. */
    float GetArrivalSongTime() const { return ArrivalSongTime; }

    /** Song time when the enemy visually lands on the red line (the beat plus its move animation). */
    float GetHitSongTime() const { return ArrivalSongTime + BeatMoveDuration; }

    UFUNCTION(BlueprintPure, Category = "Enemy")
    bool IsInAttackRange() const;

    /** Called by the character when the player kills it in the success zone. */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void Kill();

    virtual bool RequiresAirAttack() const { return false; }
 

protected:
    /** Called when it reaches the core. Damage the core here once the core exists. */
    void ReachCore();

    /** Called when the music manager detects a new beat. */
    UFUNCTION()
    void HandleBeat(int32 BeatNumber);


    virtual void UpdateEnemyPosition();

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

    /** How long the movement animation lasts after each beat.*/
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.01", Units = "s"))
    float BeatMoveDuration = 0.15f;

    /** Number of beats between each movement step. */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "1"))
    int32 BeatsPerMove = 1;

    // Counts beats since this enemy spawned.
    int32 BeatsSinceSpawn = 0;

    // Tracks whether this enemy has already reached the red line.
    bool bHasReachedHitZone = false;

    FVector Center = FVector::ZeroVector;
    float LaneAngleDeg = 0.f;
    float SpawnRadius = 1500.f;
    float HitRadius = 500.f;
    float ArrivalSongTime = 0.f;
    float TravelTime = 2.f;

    /** Current and target positions along the enemy's lane. */
    float CurrentRadius = 1500.f;
    float StartRadius = 1500.f;
    float TargetRadius = 1500.f;

    /** Distance traveled per beat. */
    float StepDistance = 100.f;

    /** Number of beats required to reach the hit zone. */
    int32 BeatsToHitZone = 1;

    /** Last beat processed by this enemy. */
    int32 LastProcessedBeat = -1;

    /** Animation progress between beats. */
    float MoveElapsed = 0.f;

    bool bMoving = false;
    bool bReachedCore = false;
    bool bInAttackRange = false;

};