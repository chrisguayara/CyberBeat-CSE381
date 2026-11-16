#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatCore.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class UStaticMesh;

UCLASS()
class CYBERBEAT_API ACyberBeatCore : public AActor
{
	GENERATED_BODY()

public:
	ACyberBeatCore();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CoreMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	/** Optional override. Leave empty to use the engine sphere. */
	UPROPERTY(EditAnywhere, Category = "Look")
	TObjectPtr<UStaticMesh> MeshOverride;

	UPROPERTY(EditAnywhere, Category = "Look")
	TObjectPtr<UMaterialInterface> CoreMaterial;

	/** Diameter in cm. Keep it under the track's InnerRadius * 2 (default 240). */
	UPROPERTY(EditAnywhere, Category = "Look", meta = (ClampMin = "10.0", Units = "cm"))
	float CoreDiameter = 100.f;

	virtual void OnConstruction(const FTransform& Transform) override;
};