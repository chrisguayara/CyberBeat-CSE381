#include "CyberBeatBasicEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
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
        Mesh->SetRelativeScale3D(FVector(1.f));  
    }
}

void ACyberBeatBasicEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void ACyberBeatBasicEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const FVector Current = GetActorLocation();
    FVector ToTarget = TargetLocation - Current;
    ToTarget.Z = 0.f;   // stay on the track plane

    if (ToTarget.Size() <= HitDistance)
    {
        ReachCore();
        return;
    }

    const FVector Step = ToTarget.GetSafeNormal() * MoveSpeed * DeltaTime;
    SetActorLocation(Current + Step);
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