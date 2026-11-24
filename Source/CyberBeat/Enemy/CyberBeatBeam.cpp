#include "Enemy/CyberBeatBeam.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ACyberBeatBeam::ACyberBeatBeam()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCastShadow(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded()) { Mesh->SetStaticMesh(CubeMesh.Object); }
}

void ACyberBeatBeam::Init(const FVector& Center, float AngleDeg, float Length, float Width,
                          UMaterialInterface* Material, const FLinearColor& InColor, float InDuration)
{
    Duration = FMath::Max(InDuration, 0.05f);

    SetActorLocation(Center);
    SetActorRotation(FRotator(0.f, AngleDeg, 0.f));
    SetActorScale3D(FVector(Length / 100.f, Width / 100.f, 0.1f));   // the cube is 100 cm per side

    if (Material) { Mesh->SetMaterial(0, Material); }
    MID = Mesh->CreateDynamicMaterialInstance(0);
    if (MID)
    {
        MID->SetVectorParameterValue(TEXT("GlowColor"), InColor);
        MID->SetScalarParameterValue(TEXT("Intensity"), PeakIntensity);
    }
}

void ACyberBeatBeam::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    Elapsed += DeltaSeconds;
    const float Alpha = 1.f - FMath::Clamp(Elapsed / Duration, 0.f, 1.f);

    if (MID) { MID->SetScalarParameterValue(TEXT("Intensity"), PeakIntensity * Alpha); }

    if (Elapsed >= Duration) { Destroy(); }
}