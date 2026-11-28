// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "Blueprint/UserWidget.h"
#include "CyberBeatCharacter.generated.h"
class UCyberBeatCountdownWidget;


class UCyberBeatHUDWidget;
class UCameraComponent;
class USpringArmComponent;
class ACyberBeatMusicManager;
class USoundBase;
class ACyberBeatTrack;

class UMaterialInterface;

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

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> MissSound;

	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	TObjectPtr<UNiagaraSystem> PerfectHitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	TObjectPtr<UNiagaraSystem> GoodHitEffect;

	/** Camera shake on a miss, in cm. Tune up if you can't see it. */
	UPROPERTY(EditDefaultsOnly, Category = "Feedback", meta = (ClampMin = "0.0"))
	float MissShakeStrength = 40.f;

	UPROPERTY(EditDefaultsOnly, Category = "Feedback", meta = (ClampMin = "0.05", Units = "s"))
	float MissShakeDuration = 0.25f;

	UPROPERTY()
	TObjectPtr<ACyberBeatTrack> Track;

	int32 CurrentLane = -1;
	float ShakeTimeLeft = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Feedback") TObjectPtr<UNiagaraSystem> DustEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Feedback") TObjectPtr<UMaterialInterface> DashTrailMaterial;
    UPROPERTY(EditDefaultsOnly, Category = "Feedback") FLinearColor DashTrailColor = FLinearColor(0.8f, 0.95f, 1.f);
    UPROPERTY(EditDefaultsOnly, Category = "Feedback") float DashTrailWidth = 25.f;
    UPROPERTY(EditDefaultsOnly, Category = "Feedback") float DashTrailDuration = 0.12f;

    void SpawnTeleportFX(const FVector& From, const FVector& To);

	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UCyberBeatCountdownWidget> CountdownWidgetClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (ClampMin = "1")) int32 CountdownSeconds = 3;

    UPROPERTY() TObjectPtr<UCyberBeatCountdownWidget> CountdownWidget;
    bool bCountingDown = false;
    float CountdownRemaining = 0.f;
    int32 LastCountdownNumber = 0;

    void StartCountdown();
    void TickCountdown(float DeltaSeconds);
	
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
	bool CanPause() const { return !bGameEnded && !bCountingDown; }
};