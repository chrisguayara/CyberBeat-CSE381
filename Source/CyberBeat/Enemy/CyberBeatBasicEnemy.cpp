#include "CyberBeatBasicEnemy.h"
#include "CyberBeatMusicManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

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
}

void ACyberBeatBasicEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(Manager)) return;

    // constant speed: it covers (SpawnRadius - HitRadius) in TravelTime seconds and is
    // exactly at HitRadius when the song clock reaches ArrivalSongTime.
    const float Now = Manager->GetSongTimeSeconds();
    const float Speed = (SpawnRadius - HitRadius) / TravelTime;
    const float Radius = HitRadius + (ArrivalSongTime - Now) * Speed;

    if (Radius <= CoreRadius)
    {
        ReachCore();
        return;
    }

    const float Rad = FMath::DegreesToRadians(LaneAngleDeg);
    SetActorLocation(Center + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0.f) * Radius);
}

void ACyberBeatBasicEnemy::Kill()
{
    Destroy();
}

void ACyberBeatBasicEnemy::ReachCore()
{
    // TODO: once the core exists, apply Damage to it here.
    UE_LOG(LogTemp, Warning, TEXT("%s reached the core (damage %d)"), *GetName(), Damage);
    Destroy();
}