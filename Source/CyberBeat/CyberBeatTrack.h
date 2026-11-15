// CyberBeatTrack.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatTrack.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;

/**
 * The 8-lane circular track. Lane centers are at 0, 45, 90 ... degrees
 * (0 = world +X = the W key), and the dividers sit between them.
 * Place one at the middle of the arena. The character and the spawner read
 * their center and radius from it, so there is one source of truth.
 */
UCLASS()
class ACyberBeatTrack : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatTrack();

    virtual void OnConstruction(const FTransform& Transform) override;

    /** Radius of the ring the player stands on and enemies must be hit at. */
    float GetTrackRadius() const { return TrackRadius; }

    static constexpr int32 NumLanes = 8;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Disc;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TArray<TObjectPtr<UStaticMeshComponent>> Dividers;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TArray<TObjectPtr<UStaticMeshComponent>> HitMarkers;

    /** Where the player stands and where the success zone is. */
    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "100.0", Units = "cm"))
    float TrackRadius = 500.f;

    /** How far the disc extends past the ring. */
    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "0.0", Units = "cm"))
    float EdgeMargin = 150.f;

    /** Dividers start this far from the center (leave room for the core). */
    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "0.0", Units = "cm"))
    float InnerRadius = 120.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "1.0", Units = "cm"))
    float DiscThickness = 20.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "1.0", Units = "cm"))
    float DividerWidth = 6.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "1.0", Units = "cm"))
    float DividerHeight = 8.f;

    UPROPERTY(EditAnywhere, Category = "Track", meta = (ClampMin = "1.0", Units = "cm"))
    float MarkerLength = 40.f;

    UPROPERTY(EditAnywhere, Category = "Look")
    TObjectPtr<UMaterialInterface> DiscMaterial;

    UPROPERTY(EditAnywhere, Category = "Look")
    TObjectPtr<UMaterialInterface> DividerMaterial;

    UPROPERTY(EditAnywhere, Category = "Look")
    TObjectPtr<UMaterialInterface> MarkerMaterial;
};