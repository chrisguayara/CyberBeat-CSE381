// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/CyberBeatFlyingEnemy.h"
#include "CyberBeatMusicManager.h"
#include "Components/StaticMeshComponent.h"

ACyberBeatFlyingEnemy::ACyberBeatFlyingEnemy()
{
    Mesh->SetRelativeScale3D(FVector(0.6f, 1.2f, 0.4f));
}

void ACyberBeatFlyingEnemy::UpdateEnemyPosition()
{
    const float Rad = FMath::DegreesToRadians(LaneAngleDeg);
    const FVector Direction(FMath::Cos(Rad), FMath::Sin(Rad), 0.f);

    SetActorLocation(Center + Direction * CurrentRadius + FVector(0.f, 0.f, FlyHeight));
}

