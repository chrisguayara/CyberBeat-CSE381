// CyberBeatTrack.cpp
#include "CyberBeatTrack.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ACyberBeatTrack::ACyberBeatTrack()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Engine shapes: Cylinder is 100cm across x 100cm tall, Cube is 100cm per side, both centered on their origin.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

    Disc = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Disc"));
    Disc->SetupAttachment(Root);
    Disc->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (CylinderMesh.Succeeded())
    {
        Disc->SetStaticMesh(CylinderMesh.Object);
    }

    for (int32 i = 0; i < NumLanes; ++i)
    {
        UStaticMeshComponent* Divider = CreateDefaultSubobject<UStaticMeshComponent>(
            FName(*FString::Printf(TEXT("Divider%d"), i)));
        Divider->SetupAttachment(Root);
        Divider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (CubeMesh.Succeeded())
        {
            Divider->SetStaticMesh(CubeMesh.Object);
        }
        Dividers.Add(Divider);

        UStaticMeshComponent* Marker = CreateDefaultSubobject<UStaticMeshComponent>(
            FName(*FString::Printf(TEXT("HitMarker%d"), i)));
        Marker->SetupAttachment(Root);
        Marker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (CubeMesh.Succeeded())
        {
            Marker->SetStaticMesh(CubeMesh.Object);
        }
        HitMarkers.Add(Marker);
    }
}

void ACyberBeatTrack::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    const float OuterRadius = TrackRadius + EdgeMargin;

    // Flat disc whose top face sits at local Z = 0.
    Disc->SetRelativeScale3D(FVector(OuterRadius * 2.f / 100.f, OuterRadius * 2.f / 100.f, DiscThickness / 100.f));
    Disc->SetRelativeLocation(FVector(0.f, 0.f, -DiscThickness * 0.5f));
    if (DiscMaterial)
    {
        Disc->SetMaterial(0, DiscMaterial);
    }

    const float Span = FMath::Max(OuterRadius - InnerRadius, 1.f);
    const float MidRadius = InnerRadius + Span * 0.5f;

    // Lane width at the ring, used to size the hit markers.
    const float LaneWidth = 2.f * TrackRadius * FMath::Sin(FMath::DegreesToRadians(22.5f));

    for (int32 i = 0; i < NumLanes; ++i)
    {
        // Lane centers are at i * 45 degrees; the dividers sit on the boundaries between them.
        const float BoundaryDeg = i * 45.f + 22.5f;
        const float BoundaryRad = FMath::DegreesToRadians(BoundaryDeg);

        if (Dividers.IsValidIndex(i) && Dividers[i])
        {
            Dividers[i]->SetRelativeLocation(
                FVector(FMath::Cos(BoundaryRad), FMath::Sin(BoundaryRad), 0.f) * MidRadius
                + FVector(0.f, 0.f, DividerHeight * 0.5f));
            Dividers[i]->SetRelativeRotation(FRotator(0.f, BoundaryDeg, 0.f));
            Dividers[i]->SetRelativeScale3D(FVector(Span / 100.f, DividerWidth / 100.f, DividerHeight / 100.f));
            if (DividerMaterial)
            {
                Dividers[i]->SetMaterial(0, DividerMaterial);
            }
        }

        const float LaneDeg = i * 45.f;
        const float LaneRad = FMath::DegreesToRadians(LaneDeg);

        if (HitMarkers.IsValidIndex(i) && HitMarkers[i])
        {
            HitMarkers[i]->SetRelativeLocation(
                FVector(FMath::Cos(LaneRad), FMath::Sin(LaneRad), 0.f) * TrackRadius
                + FVector(0.f, 0.f, 2.f));
            HitMarkers[i]->SetRelativeRotation(FRotator(0.f, LaneDeg, 0.f));
            HitMarkers[i]->SetRelativeScale3D(FVector(MarkerLength / 100.f, LaneWidth * 0.6f / 100.f, 4.f / 100.f));
            if (MarkerMaterial)
            {
                HitMarkers[i]->SetMaterial(0, MarkerMaterial);
            }
        }
    }
}