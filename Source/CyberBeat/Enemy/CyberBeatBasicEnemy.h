#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatBasicEnemy.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class ACyberBeatMusicManager;
class UMaterialInterface; 
class UMaterialInstanceDynamic; 
class USoundBase;


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
    float GetHitSongTime() const { return ArrivalSongTime ;}

    UFUNCTION(BlueprintPure, Category = "Enemy")
    bool IsInAttackRange() const;

    /** Called by the character when the player kills it in the success zone. */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    virtual void Kill();

    virtual bool RequiresAirAttack() const { return false; }

    virtual void BeginPlay() override;
    void SetGlowTarget(float InTarget) { GlowTarget = InTarget; }

    void MakePowerup();
    bool IsPowerup() const { return bIsPowerup; }

   
 

protected:
    /** Called when it reaches the core. Damage the core here once the core exists. */
    void ReachCore();

    
    


    virtual void UpdateEnemyPosition();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Ring;

    UPROPERTY(EditDefaultsOnly, Category = "Ring") TObjectPtr<UMaterialInterface> RingMaterial;
    UPROPERTY(EditDefaultsOnly, Category = "Ring") FLinearColor RingColor = FLinearColor(1.f, 0.2f, 0.1f);
    UPROPERTY(EditDefaultsOnly, Category = "Ring") FLinearColor ChargedRingColor = FLinearColor(0.3f, 0.8f, 1.f);
    UPROPERTY(EditDefaultsOnly, Category = "Ring") float RingSize = 160.f;        // cm diameter
    UPROPERTY(EditDefaultsOnly, Category = "Ring") float RingZOffset = -95.f;     // relative to enemy center
    UPROPERTY(EditDefaultsOnly, Category = "Ring") float RingBaseIntensity = 2.f;
    UPROPERTY(EditDefaultsOnly, Category = "Ring") float RingGlowBoost = 6.f;     // added by the "next enemy" glow

    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> RingMID;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> ChargeShell;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane Switching")
    bool bCanSwitchLanes = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane Switching")
    float LaneSwitchDuration = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane Switching")
    float LaneSwitchProgress = 0.2f;

    bool bHasSwitchedLanes = false;
    bool bIsSwitchingLanes = false;

    float SwitchStartAngle = 0.f;
    float SwitchTargetAngle = 0.f;
    float SwitchElapsedTime = 0.f;

    void StartLaneSwitch();
    void UpdateLaneSwitch(float DeltaTime);

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

    UPROPERTY(EditAnywhere, Category = "Movement", meta = (Units = "s"))
    float AttackRangeSeconds = 0.15f;

    UPROPERTY(EditDefaultsOnly, Category = "Look")
    TObjectPtr<UMaterialInterface> GlowMaterial;   // an instance of M_Glow

    UPROPERTY(EditDefaultsOnly, Category = "Look")
    FLinearColor GlowColor = FLinearColor(1.f, 0.2f, 0.1f);

    UPROPERTY(EditDefaultsOnly, Category = "Look")
    float BaseIntensity = 0.6f;

    UPROPERTY(EditDefaultsOnly, Category = "Look")
    float MaxGlowIntensity = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Look")
    float GlowFadeSpeed = 8.f;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundBase> MoveSound;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    float MoveSoundVolume = 1.f;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MID;

    float GlowTarget = 0.f;
    float GlowCurrent = 0.f;
    float StepInterval = 0.5f;
    int32 TotalSteps = 1;
    int32 LastHopLevel = INT32_MAX;

    void Detonate();

    bool bIsPowerup = false;
    float PowerupTime = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = "Powerup") TObjectPtr<USoundBase> BlastSound;
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") TObjectPtr<UMaterialInterface> BeamMaterial; // M_Glow instance
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") FLinearColor BeamColor = FLinearColor(0.3f, 0.9f, 1.f);
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") float BeamWidth = 80.f;
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") float BeamDuration = 0.35f;
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") FLinearColor RimColor = FLinearColor(1.f, 0.85f, 0.2f);
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") float RimMax = 3.f;
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") float RimPulseSpeed = 2.f;   // pulses per second
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") float PowerupScale = 1.25f;

    UPROPERTY(EditDefaultsOnly, Category = "Powerup") TObjectPtr<UMaterialInterface> ChargeMaterial; // M_ChargeShell instance
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") FLinearColor ChargeColor = FLinearColor(0.3f, 0.7f, 1.f);
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") float ChargeShellScale = 1.2f;
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") float ChargeIntensity = 4.f;
    /** If this enemy hits the core while charged, damage is multiplied by this. */
    UPROPERTY(EditDefaultsOnly, Category = "Powerup", meta = (ClampMin = "1")) int32 ChargedCoreDamageMultiplier = 3;
    /** Beam also kills enemies in the lane it was fired from (it visually spans both sides). */
    UPROPERTY(EditDefaultsOnly, Category = "Powerup") bool bBeamKillsOwnLane = true;

    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> ShellMID;


};