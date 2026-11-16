#include "CyberBeatCore.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ACyberBeatCore::ACyberBeatCore()
{
	PrimaryActorTick.bCanEverTick = false;

	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	SetRootComponent(CoreMesh);
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
	}
	if (CoreMaterial)
	{
		CoreMesh->SetMaterial(0, CoreMaterial);
	}
	// The engine sphere is 100cm across. If you use your own mesh, set the scale by hand instead.
	if (!MeshOverride)
	{
		CoreMesh->SetRelativeScale3D(FVector(CoreDiameter / 100.f));
	}
}

// Called when the game starts or when spawned
void ACyberBeatCore::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACyberBeatCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

