// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberBeatCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"

ACyberBeatCharacter::ACyberBeatCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create the camera boom component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACyberBeatCharacter::BeginPlay()
{
	Super::BeginPlay();

	Super::BeginPlay();

	GetCharacterMovement()->SetMovementMode(MOVE_None);   

	const FVector Rel = GetActorLocation() - TrackCenter;
	CurrentAngle = TargetAngle = FMath::RadiansToDegrees(FMath::Atan2(Rel.Y, Rel.X));
}

void ACyberBeatCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	const float X = (PC->IsInputKeyDown(EKeys::W) ? 1.f : 0.f) - (PC->IsInputKeyDown(EKeys::S) ? 1.f : 0.f);
	const float Y = (PC->IsInputKeyDown(EKeys::D) ? 1.f : 0.f) - (PC->IsInputKeyDown(EKeys::A) ? 1.f : 0.f);

	if (X != 0.f || Y != 0.f)
	{
		TargetAngle = FMath::RadiansToDegrees(FMath::Atan2(Y, X)) + LaneYawOffset;
	}
	CurrentAngle = FMath::FixedTurn(CurrentAngle, TargetAngle, TurnSpeed * DeltaSeconds);

	const float Rad = FMath::DegreesToRadians(CurrentAngle);
	FVector Loc = TrackCenter + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0.f) * TrackRadius;
	Loc.Z = GetActorLocation().Z;
	SetActorLocation(Loc);
	SetActorRotation(FRotator(0.f, CurrentAngle, 0.f));
}
