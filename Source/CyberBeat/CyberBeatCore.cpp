#include "CyberBeatCore.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ACyberBeatCore::ACyberBeatCore()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	CoreMesh->SetupAttachment(Root);
	CoreMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
		// Your own mesh keeps whatever size it was authored at; scale the actor in the level.
		CoreMesh->SetRelativeScale3D(FVector::OneVector);
	}
	else
	{
		// The engine sphere is 100 cm across.
		CoreMesh->SetRelativeScale3D(FVector(CoreDiameter / 100.f));
	}

	if (CoreMaterial)
	{
		CoreMesh->SetMaterial(0, CoreMaterial);
	}
}