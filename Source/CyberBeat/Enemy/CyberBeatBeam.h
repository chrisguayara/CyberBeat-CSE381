#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatBeam.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/** A glowing strip that flashes along a lane axis and fades out. */
UCLASS()
class CYBERBEAT_API ACyberBeatBeam : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatBeam();

    void Init(const FVector& Center, float AngleDeg, float Length, float Width,
              UMaterialInterface* Material, const FLinearColor& InColor, float InDuration);

    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MID;

    float Duration = 0.35f;
    float Elapsed = 0.f;
    float PeakIntensity = 25.f;
};