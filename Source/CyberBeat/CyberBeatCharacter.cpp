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
#include "CyberBeatMusicManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

	MusicManager = Cast<ACyberBeatMusicManager>(
		UGameplayStatics::GetActorOfClass(
			GetWorld(),
			ACyberBeatMusicManager::StaticClass()
		)
	);

	if (MusicManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Music Manager exists"));
	}
	


	GetCharacterMovement()->SetMovementMode(MOVE_None);   

	const FVector Rel = GetActorLocation() - TrackCenter;
	CurrentAngle = TargetAngle = FMath::RadiansToDegrees(FMath::Atan2(Rel.Y, Rel.X));
}

void ACyberBeatCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	const bool bW = PC->IsInputKeyDown(EKeys::W);
	const bool bA = PC->IsInputKeyDown(EKeys::A);
	const bool bS = PC->IsInputKeyDown(EKeys::S);
	const bool bD = PC->IsInputKeyDown(EKeys::D);

	const bool bAttack = PC->IsInputKeyDown(EKeys::LeftMouseButton);
	const bool bNewAttack = bAttack && !bAttackWasDown;

	bAttackWasDown = bAttack;

	if (bNewAttack)
	{
		TryAttack();
	}

	const bool bNewW = bW && !bWWasDown;
	const bool bNewA = bA && !bAWasDown;
	const bool bNewS = bS && !bSWasDown;
	const bool bNewD = bD && !bDWasDown;

	bWWasDown = bW;
	bAWasDown = bA;
	bSWasDown = bS;
	bDWasDown = bD;

	if (bNewW || bNewA || bNewS || bNewD)
	{
		float X = 0.f;
		float Y = 0.f;

		if (bW) X += 1.f;
		if (bS) X -= 1.f;
		if (bD) Y += 1.f;
		if (bA) Y -= 1.f;

		if (X != 0.f || Y != 0.f)
		{
			TargetAngle = FMath::RadiansToDegrees(FMath::Atan2(Y, X)) + LaneYawOffset;
		}
	}
	CurrentAngle = FMath::FixedTurn(CurrentAngle, TargetAngle, TurnSpeed * DeltaSeconds);

	const float Rad = FMath::DegreesToRadians(CurrentAngle);
	FVector Loc = TrackCenter + FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0.f) * TrackRadius;
	Loc.Z = GetActorLocation().Z;
	SetActorLocation(Loc);
	SetActorRotation(FRotator(0.f, CurrentAngle, 0.f));
}

void ACyberBeatCharacter::TryAttack()
{
	PlayAttackEffect();

	if (!MusicManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Music Manager is missing"));
		return;
	}

	float TimingOffset = 0.0f;

	EBeatTimingResult Result = MusicManager->JudgeInput(TimingOffset);

	switch (Result)
	{
	case EBeatTimingResult::Perfect:
		UE_LOG(LogTemp, Warning,
			TEXT("Perfect, Timing offset: %.1f ms"),
			TimingOffset);
		break;

	case EBeatTimingResult::Good:
		UE_LOG(LogTemp, Warning,
			TEXT("Good, Timing offset: %.1f ms"),
			TimingOffset);
		break;

	case EBeatTimingResult::Miss:
		UE_LOG(LogTemp, Error,
			TEXT("Miss, Timing offset: %.1f ms"),
			TimingOffset);
		break;
	}
}

void ACyberBeatCharacter::PlayAttackEffect()
{
	if (!AttackSlashEffect)
	{
		return;
	}

	FVector Forward = GetActorForwardVector();

	FVector SpawnLocation = GetActorLocation() + Forward * 120.0f;
	SpawnLocation.Z += 80.0f;

	FRotator SpawnRotation = GetActorRotation();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),AttackSlashEffect,SpawnLocation,SpawnRotation,FVector(1.0f),true,true,ENCPoolMethod::None);

}