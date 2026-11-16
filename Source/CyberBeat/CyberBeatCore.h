#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatCore.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UPointLightComponent;
class UMaterialInterface;
class UStaticMesh;
class USoundBase;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoreHealthChanged, int32, Health, int32, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoreDestroyed);

UCLASS()
class CYBERBEAT_API ACyberBeatCore : public AActor
{
	GENERATED_BODY()

public:
	ACyberBeatCore();

	/** Call this whenever an enemy reaches the core. */
	UFUNCTION(BlueprintCallable, Category = "Core")
	void PlayHitReaction();

	virtual void BeginPlay() override;

	/** Enemies call this when they reach the core. */
	void ApplyDamage(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Core")
	int32 GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Core")
	int32 GetMaxHealth() const { return MaxHealth; }

	UPROPERTY(BlueprintAssignable, Category = "Core")
	FOnCoreHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Core")
	FOnCoreDestroyed OnCoreDestroyed;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CoreMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPointLightComponent> HitLight;

	/** Optional override. Leave empty to use the engine sphere. */
	UPROPERTY(EditAnywhere, Category = "Look")
	TObjectPtr<UStaticMesh> MeshOverride;

	UPROPERTY(EditAnywhere, Category = "Look")
	TObjectPtr<UMaterialInterface> CoreMaterial;

	/** Diameter in cm of the engine sphere. Ignored when MeshOverride is set. */
	UPROPERTY(EditAnywhere, Category = "Look", meta = (ClampMin = "10.0", Units = "cm"))
	float CoreDiameter = 160.f;

	/** Played when an enemy hits the core. */
	UPROPERTY(EditAnywhere, Category = "Hit")
	TObjectPtr<USoundBase> CoreHitSound;

	/** Optional burst of particles at the core. */
	UPROPERTY(EditAnywhere, Category = "Hit")
	TObjectPtr<UNiagaraSystem> CoreHitEffect;

	/** The core swells to this multiple of its size, then shrinks back. */
	UPROPERTY(EditAnywhere, Category = "Hit", meta = (ClampMin = "1.0"))
	float PunchScale = 1.35f;

	/** Peak brightness of the red flash. Raise it if you can't see it. */
	UPROPERTY(EditAnywhere, Category = "Hit")
	float FlashIntensity = 20000.f;

	UPROPERTY(EditAnywhere, Category = "Hit", meta = (ClampMin = "0.05", Units = "s"))
	float FlashDuration = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Health", meta = (ClampMin = "1"))
	int32 MaxHealth = 10;
private:
	FVector BaseMeshScale = FVector::OneVector;
	float FlashTimeLeft = 0.f;


	int32 CurrentHealth = 0;
};