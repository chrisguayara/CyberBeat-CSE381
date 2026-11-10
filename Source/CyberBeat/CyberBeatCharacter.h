// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CyberBeatCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 *  A controllable top-down perspective character
 */
UCLASS(abstract)
class ACyberBeatCharacter : public ACharacter
{
	GENERATED_BODY()
protected:

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

    float CurrentAngle = 0.f;
    float TargetAngle = 0.f;


	bool bWWasDown = false;
	bool bAWasDown = false;
	bool bSWasDown = false;
	bool bDWasDown = false;
	
private:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

public:

	/** Constructor */
	ACyberBeatCharacter();

	/** Initialization */
	virtual void BeginPlay() override;

	/** Update */
	virtual void Tick(float DeltaSeconds) override;

	/** Returns the camera component **/
	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }

	/** Returns the Camera Boom component **/
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

};

