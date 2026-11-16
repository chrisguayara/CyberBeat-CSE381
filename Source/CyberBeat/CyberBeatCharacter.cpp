// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberBeatCharacter.h"
#include "CyberBeatCore.h"
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
#include "CyberBeatPlayerState.h"
#include "CyberBeatGameOverWidget.h"
#include "NiagaraComponent.h"
#include "Engine/Engine.h"
#include "CyberBeatHUDWidget.h"
#include "EngineUtils.h"             
#include "Enemy/CyberBeatBasicEnemy.h"
#include "CyberBeatTrack.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundBase.h"

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
		MusicManager->OnSongFinished.AddDynamic(this, &ACyberBeatCharacter::HandleSongFinished);
		UE_LOG(LogTemp, Warning, TEXT("Music Manager exists"));
	}

	if (ACyberBeatCore* Core = Cast<ACyberBeatCore>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ACyberBeatCore::StaticClass())))
	{
		Core->OnCoreDestroyed.AddDynamic(this, &ACyberBeatCharacter::HandleCoreDestroyed);
	}


	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UCyberBeatHUDWidget>(
			PC,HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
	


	GetCharacterMovement()->SetMovementMode(MOVE_None);   

	if (const ACyberBeatTrack* Track = Cast<ACyberBeatTrack>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ACyberBeatTrack::StaticClass())))
	{
		TrackCenter = Track->GetActorLocation();
		TrackRadius = Track->GetPlayerRadius();
	}

	

	// camera stays over the core instead of following you around the ring
	CameraBoom->SetUsingAbsoluteLocation(true);
	CameraBoom->SetWorldLocation(TrackCenter);
	CameraBoom->SetWorldRotation(FRotator(-80.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 1800;

	const FVector Rel = GetActorLocation() - TrackCenter;
	CurrentAngle = TargetAngle = FMath::RadiansToDegrees(FMath::Atan2(Rel.Y, Rel.X));
}

void ACyberBeatCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	// Update on-screen beat indicator
	if (MusicManager && GEngine)
	{
		const float BPM = MusicManager->GetBPM();
		const float SongTime = MusicManager->GetSongTimeSeconds();

		if (BPM > 0.0f)
		{
		
			float BeatProgress = FMath::Fmod(SongTime * BPM / 60.0f,1.0f);

			if (BeatProgress < 0.0f)
			{
				BeatProgress += 1.0f;
			}

			// Move indicator toward the center
			const int32 MarkerPosition = FMath::Clamp(
				FMath::RoundToInt(BeatProgress * 10.0f),0,10);

			FString TimingBar = TEXT("[");

			for (int32 i = 0; i <= 20; i++)
			{
				if (i == MarkerPosition)
				{
					TimingBar += TEXT("O");
				}
				else if (i == 10)
				{
					TimingBar += TEXT("|");
				}
				else
				{
					TimingBar += TEXT("-");
				}
			}

			TimingBar += TEXT("]");

			FString IndicatorText = FString::Printf(
				TEXT("NEXT BEAT  %s "),*TimingBar);

			GEngine->AddOnScreenDebugMessage(9001,0.1f,FColor::Cyan,IndicatorText);
		}
	}

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
	if (bGameEnded) return;
	PlayAttackEffect();

	if (!MusicManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Music Manager is missing"));
		return;
	}

	const float Now = MusicManager->GetSongTimeSeconds();

	// finds the enemy in my lane whose arrival time is closest to now
	ACyberBeatBasicEnemy* Best = nullptr;
	float BestDiff = TNumericLimits<float>::Max();
	for (TActorIterator<ACyberBeatBasicEnemy> It(GetWorld()); It; ++It)
	{
		ACyberBeatBasicEnemy* Enemy = *It;
		const float LaneDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(TargetAngle, Enemy->GetLaneAngleDeg()));
		if (LaneDelta > 22.5f) continue;   // not my lane

		const float Diff = FMath::Abs(Now - Enemy->GetArrivalSongTime());
		if (Diff < BestDiff) { BestDiff = Diff; Best = Enemy; }
	}

	if (SwingSound) { UGameplayStatics::PlaySound2D(this, SwingSound); }

	EBeatTimingResult Result = EBeatTimingResult::Miss;   // no enemy in my lane = whiff
	float OffsetMs = 0.f;
	if (Best)
	{
		Result = MusicManager->JudgeAgainstTime(Best->GetArrivalSongTime(), OffsetMs);
		if (Result != EBeatTimingResult::Miss)
		{
			Best->Kill();
			if (HitSound) { UGameplayStatics::PlaySound2D(this, HitSound); }
		}
	}

	if (HUDWidget) { HUDWidget->ShowTiming(Result); }

	if (ACyberBeatPlayerState* ScoreState = GetPlayerState<ACyberBeatPlayerState>())
	{
		ScoreState->RegisterHit(Result);
	}

	UE_LOG(LogTemp, Warning, TEXT("Attack: %s, offset %.1f ms, enemy %s"),
		Result == EBeatTimingResult::Perfect ? TEXT("Perfect") :
		Result == EBeatTimingResult::Good ? TEXT("Good") : TEXT("Miss"),
		OffsetMs, Best ? TEXT("found") : TEXT("none in lane"));
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

void ACyberBeatCharacter::HandleSongFinished()
{
	if (bGameEnded) return;
	bGameEnded = true;
	ShowEndScreen(SuccessWidgetClass);
}

void ACyberBeatCharacter::HandleCoreDestroyed()
{
	if (bGameEnded) return;
	bGameEnded = true;

	if (MusicManager)
	{
		MusicManager->StartFailSequence();
	}
	ShowEndScreen(GameOverWidgetClass);
}

void ACyberBeatCharacter::ShowEndScreen(TSubclassOf<UUserWidget> WidgetClass)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !WidgetClass) return;

	if (UUserWidget* Screen = CreateWidget<UUserWidget>(PC, WidgetClass))
	{

		if (UCyberBeatGameOverWidget* GameOverScreen =Cast<UCyberBeatGameOverWidget>(Screen))
		{
			if (ACyberBeatPlayerState* ScoreState =GetPlayerState<ACyberBeatPlayerState>())
			{
				GameOverScreen->SetFinalScore(ScoreState->TotalScore);
			}
		}

		Screen->AddToViewport(100);
		PC->bShowMouseCursor = true;

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(Screen->TakeWidget());
		PC->SetInputMode(Mode);
	}
}