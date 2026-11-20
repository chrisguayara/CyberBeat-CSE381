#include "CyberBeatBasicEnemy.h"
#include "CyberBeatMusicManager.h"
#include "CyberBeatCore.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

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

    CurrentRadius = SpawnRadius;
    StartRadius = SpawnRadius;
    TargetRadius = SpawnRadius;
    MoveElapsed = 0.f;
    BeatsSinceSpawn = 0;
    bMoving = false;
    bReachedCore = false;
    bInAttackRange = false;
    bHasReachedHitZone = false;


    if (!IsValid(Manager)) return;

    // Calculate how many beats remain until the hit zone.
    const float BPM = Manager->GetBPM();
    const float RemainingTime = FMath::Max(ArrivalSongTime - Manager->GetSongTimeSeconds(),0.0f );

    const float TimeToHit = RemainingTime > 0.f ? RemainingTime: TravelTime;

    const float BeatInterval = 60.f / FMath::Max(BPM, 1.f);
    BeatsToHitZone = FMath::Max(1,FMath::RoundToInt(TimeToHit / BeatInterval));

    const int32 MovementSteps = FMath::Max(1,FMath::DivideAndRoundUp(BeatsToHitZone, BeatsPerMove));

    // Divide the distance into equal beat based steps.
    StepDistance = (SpawnRadius - HitRadius) / MovementSteps;
    LastProcessedBeat = Manager->GetCurrentBeat();
    Manager->OnBeat.AddDynamic(this, &ACyberBeatBasicEnemy::HandleBeat );

    UpdateEnemyPosition();

}

void ACyberBeatBasicEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(Manager)) return;

    // After a fail the song clock reads 0, which would fling enemies back out. Freeze them instead.
    if (Manager->IsFailed()) return;

    if (!bMoving) return;

    MoveElapsed += DeltaTime;

    const float Alpha = FMath::Clamp(MoveElapsed / BeatMoveDuration,0.f,1.f );

    CurrentRadius = FMath::Lerp(StartRadius,TargetRadius,Alpha);
    UpdateEnemyPosition();

    if (Alpha >= 1.f)
    {
        bMoving = false;
        CurrentRadius = TargetRadius;

        // The enemy has arrived at the red attack line.
        if (!bHasReachedHitZone && CurrentRadius <= HitRadius + 0.1f)
        {
            bHasReachedHitZone = true;
            bInAttackRange = true;
    
        }

        if (CurrentRadius <= CoreRadius)
        {
            ReachCore();
        }
    }
}

void ACyberBeatBasicEnemy::Kill()
{
    Destroy();
}

void ACyberBeatBasicEnemy::ReachCore()
{
    UE_LOG(LogTemp, Warning, TEXT("%s reached the core (damage %d)"), *GetName(), Damage);

    if (ACyberBeatCore* Core = Cast<ACyberBeatCore>(
            UGameplayStatics::GetActorOfClass(GetWorld(), ACyberBeatCore::StaticClass())))
    {
        Core->ApplyDamage(Damage);
    }
    Destroy();
}

void ACyberBeatBasicEnemy::HandleBeat(int32 BeatNumber)
{
    if (!IsValid(Manager) || Manager->IsFailed()) return;
    if (BeatNumber <= LastProcessedBeat) return;

    LastProcessedBeat = BeatNumber;

    BeatsSinceSpawn++;

    if (BeatsSinceSpawn < BeatsPerMove) return;

    BeatsSinceSpawn = 0;

    // If the enemy was waiting at the red line, it now moves in.
    if (bInAttackRange)
    {
        bInAttackRange = false;
    }

    if (bMoving) return;

    StartRadius = CurrentRadius;

    if (!bHasReachedHitZone)
    {
        // Approach the red line, but dont pass it
        TargetRadius = FMath::Max(HitRadius,CurrentRadius - StepDistance);

        if (TargetRadius <= HitRadius + 0.1f)
        {
            bInAttackRange = true;
        }

    }
    else
    {
        // After the attack window, continue toward the core.
        TargetRadius = FMath::Max(CoreRadius,CurrentRadius - StepDistance);
    }

    MoveElapsed = 0.f;
    bMoving = true;
}

void ACyberBeatBasicEnemy::UpdateEnemyPosition()
{
    const float Rad = FMath::DegreesToRadians(LaneAngleDeg);
    const FVector Direction(FMath::Cos(Rad), FMath::Sin(Rad), 0.f);
    SetActorLocation(Center + Direction * CurrentRadius);
}

void ACyberBeatBasicEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsValid(Manager))
    {
        Manager->OnBeat.RemoveDynamic( this,&ACyberBeatBasicEnemy::HandleBeat );
    }

    Super::EndPlay(EndPlayReason);
}


bool ACyberBeatBasicEnemy::IsInAttackRange() const
{
    return bInAttackRange;
}