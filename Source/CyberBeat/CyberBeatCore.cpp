#include "CyberBeatCore.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"

ACyberBeatCore::ACyberBeatCore()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;   // only ticks during a hit reaction

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	CoreMesh->SetupAttachment(Root);
	CoreMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HitLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HitLight"));
	HitLight->SetupAttachment(Root);
	HitLight->SetMobility(EComponentMobility::Movable);
	HitLight->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	HitLight->SetLightColor(FLinearColor::Red);
	HitLight->SetAttenuationRadius(1200.f);
	HitLight->SetCastShadows(false);
	HitLight->SetIntensity(0.f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (Sphere.Succeeded())
	{
		CoreMesh->SetStaticMesh(Sphere.Object);
	}
}

void ACyberBeatCore::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshOverride)
	{
		CoreMesh->SetStaticMesh(MeshOverride);
		CoreMesh->SetRelativeScale3D(FVector::OneVector);
	}
	else
	{
		CoreMesh->SetRelativeScale3D(FVector(CoreDiameter / 100.f));
	}
	BaseMeshScale = CoreMesh->GetRelativeScale3D();

	if (CoreMaterial)
	{
		CoreMesh->SetMaterial(0, CoreMaterial);
	}
}

void ACyberBeatCore::PlayHitReaction()
{
	FlashTimeLeft = FlashDuration;
	SetActorTickEnabled(true);

	if (CoreHitSound)
	{
		UGameplayStatics::PlaySound2D(this, CoreHitSound);
	}
	if (CoreHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CoreHitEffect, GetActorLocation());
	}
}

void ACyberBeatCore::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FlashTimeLeft = FMath::Max(FlashTimeLeft - DeltaSeconds, 0.f);
	const float Alpha = FlashTimeLeft / FlashDuration;   // 1 right after the hit, 0 when finished

	HitLight->SetIntensity(FlashIntensity * Alpha);
	CoreMesh->SetRelativeScale3D(BaseMeshScale * FMath::Lerp(1.f, PunchScale, Alpha));

	if (FlashTimeLeft <= 0.f)
	{
		SetActorTickEnabled(false);
	}
}

void ACyberBeatCore::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void ACyberBeatCore::ApplyDamage(int32 Amount)
{
	if (CurrentHealth <= 0) return;

	CurrentHealth = FMath::Max(CurrentHealth - Amount, 0);
	PlayHitReaction();
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth == 0)
	{
		OnCoreDestroyed.Broadcast();
	}
}