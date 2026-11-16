// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CyberBeatGameOverWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class CYBERBEAT_API UCyberBeatGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

    UFUNCTION(BlueprintCallable, Category = "Game Over")
    void SetFinalScore(int32 Score);

protected:

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> FinalScoreText;

};


