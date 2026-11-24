// CyberBeatTrack.cpp
#include "CyberBeatTrack.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ACyberBeatTrack::ACyberBeatTrack()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

    for (int32 i = 0; i < NumLanes; ++i)
    {
        UStaticMeshComponent* Slice = CreateDefaultSubobject<UStaticMeshComponent>(
            FName(*FString::Printf(TEXT("Slice%d"), i)));
        Slice->SetupAttachment(Root);
        Slice->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Slice->SetCastShadow(false);
        Slices.Add(Slice);

        UStaticMeshComponent* Marker = CreateDefaultSubobject<UStaticMeshComponent>(
            FName(*FString::Printf(TEXT("HitMarker%d"), i)));
        Marker->SetupAttachment(Root);
        Marker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Marker->SetCastShadow(false);
        if (CubeMesh.Succeeded()) { Marker->SetStaticMesh(CubeMesh.Object); }
        HitMarkers.Add(Marker);
    }
}

void ACyberBeatTrack::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    for (int32 i = 0; i < NumLanes; ++i)
    {
        const float LaneDeg = i * 45.f;
        const float LaneRad = FMath::DegreesToRadians(LaneDeg);

        if (Slices.IsValidIndex(i) && Slices[i])
        {
            Slices[i]->SetStaticMesh(SliceMesh);
            Slices[i]->SetRelativeLocation(FVector::ZeroVector);
            Slices[i]->SetRelativeRotation(FRotator(0.f, LaneDeg - SliceCenterYawDeg, 0.f));
            Slices[i]->SetRelativeScale3D(FVector(SliceScale));
            if (SliceMaterial) { Slices[i]->SetMaterial(0, SliceMaterial); }
        }

        if (HitMarkers.IsValidIndex(i) && HitMarkers[i])
        {
            HitMarkers[i]->SetRelativeLocation(
                FVector(FMath::Cos(LaneRad), FMath::Sin(LaneRad), 0.f) * TrackRadius
                + FVector(0.f, 0.f, 2.f));
            HitMarkers[i]->SetRelativeRotation(FRotator(0.f, LaneDeg, 0.f));
            HitMarkers[i]->SetRelativeScale3D(FVector(MarkerLength / 100.f, MarkerWidth / 100.f, 4.f / 100.f));
            if (MarkerMaterial) { HitMarkers[i]->SetMaterial(0, MarkerMaterial); }
        }
    }
}

void ACyberBeatTrack::BeginPlay()
{
    Super::BeginPlay();

    ActiveAlpha.Init(0.f, NumLanes);
    PulseAlpha.Init(0.f, NumLanes);
    PulseColor.Init(FLinearColor::White, NumLanes);

    MarkerMIDs.Reset();
    for (int32 i = 0; i < NumLanes; ++i)
    {
        MarkerMIDs.Add(HitMarkers[i] ? HitMarkers[i]->CreateDynamicMaterialInstance(0) : nullptr);
    }

    if (!SliceMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("CyberBeatTrack: assign SliceMesh"));
    }
}

void ACyberBeatTrack::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    for (int32 i = 0; i < NumLanes; ++i)
    {
        ActiveAlpha[i] = FMath::FInterpTo(ActiveAlpha[i], (i == ActiveLane) ? 1.f : 0.f, DeltaSeconds, ActiveFadeSpeed);
        PulseAlpha[i] = FMath::Max(PulseAlpha[i] - DeltaSeconds * PulseDecay, 0.f);

        if (MarkerMIDs.IsValidIndex(i) && MarkerMIDs[i])
        {
            const FLinearColor Color = FMath::Lerp(MarkerColor, PulseColor[i], PulseAlpha[i]);
            const float Intensity = FMath::Lerp(MarkerIdleIntensity, MarkerActiveIntensity, ActiveAlpha[i])
                                  + PulseAlpha[i] * PulseIntensity;
            MarkerMIDs[i]->SetVectorParameterValue(TEXT("GlowColor"), Color);
            MarkerMIDs[i]->SetScalarParameterValue(TEXT("Intensity"), Intensity);
        }
    }
}

void ACyberBeatTrack::SetActiveLane(int32 Lane)
{
    ActiveLane = Lane;
}

void ACyberBeatTrack::PulseLane(int32 Lane, EBeatTimingResult Result)
{
    if (!PulseAlpha.IsValidIndex(Lane)) return;

    switch (Result)
    {
    case EBeatTimingResult::Perfect: PulseColor[Lane] = PerfectColor; PulseAlpha[Lane] = 1.0f; break;
    case EBeatTimingResult::Good:    PulseColor[Lane] = GoodColor;    PulseAlpha[Lane] = 0.6f; break;
    default:                         PulseColor[Lane] = MissColor;    PulseAlpha[Lane] = 0.8f; break;
    }
}

FVector ACyberBeatTrack::GetLaneHitLocation(int32 Lane) const
{
    if (!HitMarkers.IsValidIndex(Lane) || !HitMarkers[Lane]) return GetActorLocation();
    return HitMarkers[Lane]->GetComponentLocation() + FVector(0.f, 0.f, 60.f);
}