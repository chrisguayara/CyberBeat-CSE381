// CyberBeatTrack.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatSongData.h"
#include "CyberBeatTrack.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class ACyberBeatTrack : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatTrack();

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    float GetTrackRadius() const { return TrackRadius; }
    float GetPlayerRadius() const { return FMath::Max(TrackRadius - PlayerInset, 0.f); }

    static constexpr int32 NumLanes = 8;

    void SetActiveLane(int32 Lane);
    void PulseLane(int32 Lane, EBeatTimingResult Result);
    FVector GetLaneHitLocation(int32 Lane) const;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TArray<TObjectPtr<UStaticMeshComponent>> Slices;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TArray<TObjectPtr<UStaticMeshComponent>> HitMarkers;

    UPROPERTY(EditAnywhere, Category = "Slice")
    TObjectPtr<UStaticMesh> SliceMesh;

    UPROPERTY(EditAnywhere, Category = "Slice")
    float SliceCenterYawDeg = 0.f;

    UPROPERTY(EditAnywhere, Category = "Slice", meta = (ClampMin = "0.01"))
    float SliceScale = 1.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "100.0", Units = "cm"))
    float TrackRadius = 500.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "0.0", Units = "cm"))
    float PlayerInset = 120.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "1.0", Units = "cm"))
    float MarkerLength = 60.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "1.0", Units = "cm"))
    float MarkerWidth = 120.f;

    /** Optional. Leave empty to keep the materials already on your slice mesh. */
    UPROPERTY(EditAnywhere, Category = "Look")
    TObjectPtr<UMaterialInterface> SliceMaterial;

    /** Instance of M_Glow (GlowColor + Intensity parameters). */
    UPROPERTY(EditAnywhere, Category = "Look")
    TObjectPtr<UMaterialInterface> MarkerMaterial;

    UPROPERTY(EditAnywhere, Category = "Glow") FLinearColor MarkerColor  = FLinearColor(0.f, 0.8f, 1.f);
    UPROPERTY(EditAnywhere, Category = "Glow") FLinearColor PerfectColor = FLinearColor(1.f, 0.85f, 0.2f);
    UPROPERTY(EditAnywhere, Category = "Glow") FLinearColor GoodColor    = FLinearColor(0.3f, 0.6f, 1.f);
    UPROPERTY(EditAnywhere, Category = "Glow") FLinearColor MissColor    = FLinearColor(1.f, 0.05f, 0.05f);

    UPROPERTY(EditAnywhere, Category = "Glow") float MarkerIdleIntensity   = 1.5f;
    UPROPERTY(EditAnywhere, Category = "Glow") float MarkerActiveIntensity = 5.f;
    UPROPERTY(EditAnywhere, Category = "Glow") float PulseIntensity        = 20.f;
    UPROPERTY(EditAnywhere, Category = "Glow") float ActiveFadeSpeed       = 25.f;
    UPROPERTY(EditAnywhere, Category = "Glow") float PulseDecay            = 4.f;

private:
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> MarkerMIDs;

    TArray<float> ActiveAlpha;
    TArray<float> PulseAlpha;
    TArray<FLinearColor> PulseColor;
    int32 ActiveLane = -1;
};