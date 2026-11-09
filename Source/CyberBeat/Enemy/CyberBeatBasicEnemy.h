#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CyberBeatBasicEnemy.generated.h"

class UStaticMeshComponent;
class USphereComponent;

/** A basic enemy: drifts in a straight line toward the core and "hits" it on arrival. */
UCLASS()
class ACyberBeatBasicEnemy : public AActor
{
    GENERATED_BODY()

public:
    ACyberBeatBasicEnemy();

    virtual void Tick(float DeltaTime) override;

    // is Called from the melee / parry code when the player kills it in the success zone. 
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void Kill();

    /** Where it is heading. The spawner sets this; default is the world origin (the core). */
    void SetTargetLocation(const FVector& NewTarget) { TargetLocation = NewTarget; }

protected:
    virtual void BeginPlay() override;

    /** Called when it reaches the core. Damage the core here once the core exists. */
    void ReachCore();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(EditAnywhere, Category = "Tuning", meta = (ClampMin = "0.0", Units = "cm/s"))
    float MoveSpeed = 150.f;

    UPROPERTY(EditAnywhere, Category = "Tuning", meta = (ClampMin = "0.0", Units = "cm"))
    float HitDistance = 120.f;

    UPROPERTY(EditAnywhere, Category = "Tuning", meta = (ClampMin = "0"))
    int32 Damage = 1;

    UPROPERTY(EditAnywhere, Category = "Tuning")
    FVector TargetLocation = FVector::ZeroVector;
};