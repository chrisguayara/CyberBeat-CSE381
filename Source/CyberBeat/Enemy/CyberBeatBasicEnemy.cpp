#include "CyberBeatBasicEnemy.h"
#include "CyberBeatMusicManager.h"
#include "CyberBeatCore.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ACyberBeatBasicEnemy::ACyberBeatBasicEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(50.f);
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    SetRootComponent(Collision);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded())
    {
        Mesh->SetStaticMesh(SphereMesh.Object);
    }
}

void ACyberBeatBasicEnemy::BeginPlay()
{
    Super::BeginPlay();

    if (GlowMaterial) { Mesh->SetMaterial(0, GlowMaterial); }
    MID = Mesh->CreateDynamicMaterialInstance(0);
    if (MID) { MID->SetVectorParameterValue(TEXT("GlowColor"), GlowColor); }
}

void ACyberBeatBasicEnemy::Init(ACyberBeatMusicManager* InManager, const FVector& InCenter, float InLaneAngleDeg,
                                float InSpawnRadius, float InHitRadius, float InArrivalSongTime, float InTravelTime)
{
    Manager = InManager;
    Center = InCenter;
    LaneAngleDeg = InLaneAngleDeg;
    SpawnRadius = InSpawnRadius;
    HitRadius = InHitRadius;
    ArrivalSongTime = InArrivalSongTime;
    TravelTime = FMath::Max(InTravelTime, 0.1f);

    const float BPM = IsValid(Manager) ? Manager->GetBPM() : 120.f;
    StepInterval = (60.f / FMath::Max(BPM, 1.f)) * FMath::Max(BeatsPerMove, 1);
    BeatMoveDuration = FMath::Min(BeatMoveDuration, StepInterval);

    TotalSteps = FMath::Max(1, FMath::CeilToInt(TravelTime / StepInterval));
    StepDistance = (SpawnRadius - HitRadius) / TotalSteps;

    CurrentRadius = SpawnRadius;
    bReachedCore = false;
    bInAttackRange = false;

    bHasSwitchedLanes = false;
    bIsSwitchingLanes = false;
    SwitchStartAngle = LaneAngleDeg;
    SwitchTargetAngle = LaneAngleDeg;
    SwitchElapsedTime = 0.f;

    UpdateEnemyPosition();
}

void ACyberBeatBasicEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    GlowCurrent = FMath::FInterpTo(GlowCurrent, GlowTarget, DeltaTime, GlowFadeSpeed);
    if (MID)
    {
        MID->SetScalarParameterValue(TEXT("Intensity"), BaseIntensity + GlowCurrent * MaxGlowIntensity);
    }

    if (!IsValid(Manager) || Manager->IsFailed()) return;

    const float Now = Manager->GetSongTimeSeconds();
    bInAttackRange = FMath::Abs(Now - ArrivalSongTime) <= AttackRangeSeconds;

    // ---- lane switch (partner's feature), now triggered by the song clock ----
    UpdateLaneSwitch(DeltaTime);

    if (bCanSwitchLanes && !bHasSwitchedLanes && !bIsSwitchingLanes)
    {
        const float TotalDistance = SpawnRadius - HitRadius;
        if (TotalDistance > 0.f)
        {
            const float Progress = (SpawnRadius - CurrentRadius) / TotalDistance;
            const float TimeRemaining = ArrivalSongTime - Now;

            // Only switch if it will finish well before the enemy lands on the red line.
            if (Progress >= LaneSwitchProgress && TimeRemaining > LaneSwitchDuration + 0.1f)
            {
                StartLaneSwitch();
            }
        }
    }

    // ---- hopping, driven by the song clock ----
    // Steps left until the red line, as a continuous number. 0 = exactly on the line at the chart time.
    const float U = (ArrivalSongTime - Now) / StepInterval;
    const int32 Level = FMath::CeilToInt(U);
    const float TimeToNextLevel = (U - (Level - 1)) * StepInterval;

    float Effective = static_cast<float>(Level);
    if (TimeToNextLevel < BeatMoveDuration)
    {
        const float A = 1.f - TimeToNextLevel / BeatMoveDuration;
        Effective -= FMath::InterpEaseOut(0.f, 1.f, A, 2.f);

        if (Level != LastHopLevel)
        {
            LastHopLevel = Level;
            // Only the nearest few enemies make noise, otherwise dense charts turn to mush.
            if (MoveSound && GlowTarget > 0.f)
            {
                UGameplayStatics::PlaySoundAtLocation(this, MoveSound, GetActorLocation(), MoveSoundVolume);
            }
        }
    }

    CurrentRadius = FMath::Min(HitRadius + Effective * StepDistance, SpawnRadius);

    if (CurrentRadius <= CoreRadius)
    {
        ReachCore();
        return;
    }

    UpdateEnemyPosition();
}

void ACyberBeatBasicEnemy::Kill()
{
    Destroy();
}

void ACyberBeatBasicEnemy::ReachCore()
{
    if (bReachedCore) return;
    bReachedCore = true;

    UE_LOG(LogTemp, Warning, TEXT("%s reached the core (damage %d)"), *GetName(), Damage);

    if (ACyberBeatCore* Core = Cast<ACyberBeatCore>(
            UGameplayStatics::GetActorOfClass(GetWorld(), ACyberBeatCore::StaticClass())))
    {
        Core->ApplyDamage(Damage);
    }
    Destroy();
}

void ACyberBeatBasicEnemy::UpdateEnemyPosition()
{
    const float Rad = FMath::DegreesToRadians(LaneAngleDeg);
    const FVector Direction(FMath::Cos(Rad), FMath::Sin(Rad), 0.f);
    SetActorLocation(Center + Direction * CurrentRadius);
}

void ACyberBeatBasicEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool ACyberBeatBasicEnemy::IsInAttackRange() const
{
    return bInAttackRange;
}

void ACyberBeatBasicEnemy::StartLaneSwitch()
{
    if (!bCanSwitchLanes || bHasSwitchedLanes) return;

    bHasSwitchedLanes = true;
    bIsSwitchingLanes = true;

    SwitchStartAngle = LaneAngleDeg;
    const float Direction = FMath::RandBool() ? 1.f : -1.f;
    SwitchTargetAngle = SwitchStartAngle + Direction * 45.f;
    SwitchElapsedTime = 0.f;
}

void ACyberBeatBasicEnemy::UpdateLaneSwitch(float DeltaTime)
{
    if (!bIsSwitchingLanes) return;

    SwitchElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(SwitchElapsedTime / FMath::Max(LaneSwitchDuration, 0.01f), 0.f, 1.f);

    LaneAngleDeg = FMath::Lerp(SwitchStartAngle, SwitchTargetAngle, Alpha);

    if (Alpha >= 1.f)
    {
        LaneAngleDeg = FMath::UnwindDegrees(SwitchTargetAngle);
        bIsSwitchingLanes = false;
    }
}