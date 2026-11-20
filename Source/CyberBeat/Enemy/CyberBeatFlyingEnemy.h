// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/CyberBeatBasicEnemy.h"
#include "CyberBeatFlyingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class CYBERBEAT_API ACyberBeatFlyingEnemy : public ACyberBeatBasicEnemy
{
	GENERATED_BODY()
	

public:
	ACyberBeatFlyingEnemy();

	virtual bool RequiresAirAttack() const override { return true; }

protected:
    virtual void UpdateEnemyPosition() override;

    UPROPERTY(EditAnywhere, Category = "Flying", meta = (ClampMin = "0.0", Units = "cm"))
    float FlyHeight = 250.f;

   
};

