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
#include "Enemy/CyberBeatBeam.h"

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

    Ring = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring"));
    Ring->SetupAttachment(Collision);
    Ring->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Ring->SetCastShadow(false);

    ChargeShell = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChargeShell"));
    ChargeShell->SetupAttachment(Mesh);          // inherits the powerup scale
    ChargeShell->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ChargeShell->SetCastShadow(false);
    ChargeShell->SetVisibility(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
    if (PlaneMesh.Succeeded()) { Ring->SetStaticMesh(PlaneMesh.Object); }
    if (SphereMesh.Succeeded()) { ChargeShell->SetStaticMesh(SphereMesh.Object); }
}

void ACyberBeatBasicEnemy::BeginPlay()
{
    Super::BeginPlay();

    if (GlowMaterial) { Mesh->SetMaterial(0, GlowMaterial); }
    MID = Mesh->CreateDynamicMaterialInstance(0);
    if (MID) { MID->SetVectorParameterValue(TEXT("GlowColor"), GlowColor); }

    if (RingMaterial)
    {
        Ring->SetMaterial(0, RingMaterial);
        Ring->SetRelativeLocation(FVector(0.f, 0.f, RingZOffset));
        Ring->SetRelativeScale3D(FVector(RingSize / 100.f));   // plane is 100x100
        RingMID = Ring->CreateDynamicMaterialInstance(0);
        if (RingMID) { RingMID->SetVectorParameterValue(TEXT("GlowColor"), RingColor); }
    }
    else
    {
        Ring->SetVisibility(false);
    }
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
    if (RingMID)
    {
        RingMID->SetScalarParameterValue(TEXT("Intensity"), RingBaseIntensity + GlowCurrent * RingGlowBoost);
    }

    if (bIsPowerup && MID)
    {
        PowerupTime += DeltaTime;
        const float Pulse = 0.5f + 0.5f * FMath::Sin(PowerupTime * RimPulseSpeed * TWO_PI);

        if (ChargeShell->IsVisible())
        {
            ChargeShell->SetRelativeRotation(FRotator(PowerupTime * 60.f, PowerupTime * 110.f, 0.f));
            ChargeShell->SetRelativeScale3D(FVector(ChargeShellScale * (1.f + 0.04f * Pulse)));
            if (ShellMID)
            {
                ShellMID->SetScalarParameterValue(TEXT("Intensity"), ChargeIntensity * (0.7f + 0.3f * Pulse));
            }
        }
        MID->SetScalarParameterValue(TEXT("RimAmount"), RimMax * (0.6f + 0.4f * Pulse));
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
    if (bIsPowerup) { Detonate(); }
    Destroy();
}

void ACyberBeatBasicEnemy::Detonate()
{
    bIsPowerup = false;   // guard against double detonation

    TArray<ACyberBeatBasicEnemy*> Victims;
    for (TActorIterator<ACyberBeatBasicEnemy> It(GetWorld()); It; ++It)
    {
        ACyberBeatBasicEnemy* Other = *It;
        if (Other == this) continue;

        const float Delta = FMath::Abs(FMath::FindDeltaAngleDegrees(LaneAngleDeg, Other->GetLaneAngleDeg()));
        const bool bOpposite = Delta > 157.5f;
        const bool bSame     = Delta < 22.5f;
        if (bOpposite || (bBeamKillsOwnLane && bSame)) { Victims.Add(Other); }
    }
    for (ACyberBeatBasicEnemy* V : Victims)
    {
        if (IsValid(V)) { V->Destroy(); }   // Destroy, not Kill, so powerups don't chain
    }

    if (ACyberBeatBeam* Beam = GetWorld()->SpawnActor<ACyberBeatBeam>(Center, FRotator::ZeroRotator))
    {
        Beam->Init(Center, LaneAngleDeg, SpawnRadius * 2.f, BeamWidth, BeamMaterial, BeamColor, BeamDuration);
    }
    if (BlastSound) { UGameplayStatics::PlaySound2D(this, BlastSound); }
}

void ACyberBeatBasicEnemy::ReachCore()
{
    if (bReachedCore) return;
    bReachedCore = true;

    const int32 Dealt = bIsPowerup ? Damage * ChargedCoreDamageMultiplier : Damage;
    UE_LOG(LogTemp, Warning, TEXT("%s reached the core (damage %d%s)"),
        *GetName(), Dealt, bIsPowerup ? TEXT(", CHARGED") : TEXT(""));

    if (ACyberBeatCore* Core = Cast<ACyberBeatCore>(
            UGameplayStatics::GetActorOfClass(GetWorld(), ACyberBeatCore::StaticClass())))
    {
        Core->ApplyDamage(Dealt);
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

void ACyberBeatBasicEnemy::MakePowerup()
{
    bIsPowerup = true;
    Mesh->SetRelativeScale3D(Mesh->GetRelativeScale3D() * PowerupScale);
    if (MID) { MID->SetVectorParameterValue(TEXT("RimColor"), RimColor); }

    if (ChargeMaterial)
    {
        ChargeShell->SetMaterial(0, ChargeMaterial);
        ChargeShell->SetRelativeScale3D(FVector(ChargeShellScale));
        ShellMID = ChargeShell->CreateDynamicMaterialInstance(0);
        if (ShellMID) { ShellMID->SetVectorParameterValue(TEXT("ChargeColor"), ChargeColor); }
        ChargeShell->SetVisibility(true);
    }
    if (RingMID) { RingMID->SetVectorParameterValue(TEXT("GlowColor"), ChargedRingColor); }
}