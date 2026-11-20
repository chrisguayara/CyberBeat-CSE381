// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "Blueprint/UserWidget.h"
#include "CyberBeatCharacter.generated.h"

class UCyberBeatHUDWidget;
class UCameraComponent;
class USpringArmComponent;
class ACyberBeatMusicManager;
class USoundBase;

UCLASS(abstract)
class ACyberBeatCharacter : public ACharacter
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SuccessWidgetClass;

	/** Set this to WBP_GameOver. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UFUNCTION()
	void HandleSongFinished();

	UFUNCTION()
	void HandleCoreDestroyed();

	void ShowEndScreen(TSubclassOf<UUserWidget> WidgetClass);

	bool bGameEnded = false;

	UPROPERTY(EditAnywhere, Category = "Track")
	float TrackRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "Track")
	FVector TrackCenter = FVector::ZeroVector;

	/** Adjust in steps of 45 if a key sends you to the wrong lane. */
	UPROPERTY(EditAnywhere, Category = "Track")
	float LaneYawOffset = 0.f;

	/** Degrees per second. High = nearly instant lane changes. */
	UPROPERTY(EditAnywhere, Category = "Track")
	float TurnSpeed = 720.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraDistance = 2800.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraPitch = -90.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraFOV = 70.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FVector CameraOffset = FVector::ZeroVector;

	float CurrentAngle = 0.f;
	float TargetAngle = 0.f;

	bool bWWasDown = false;
	bool bAWasDown = false;
	bool bSWasDown = false;
	bool bDWasDown = false;
	bool bAirAttackWasDown = false;

	UPROPERTY()
	TObjectPtr<ACyberBeatMusicManager> MusicManager;

	bool bAttackWasDown = false;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> AttackSlashEffect;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCyberBeatHUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UCyberBeatHUDWidget> HUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> SwingSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> HitSound;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

public:

	ACyberBeatCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

	void TryAttack(bool bAirAttack = false);
	void PlayAttackEffect();
};