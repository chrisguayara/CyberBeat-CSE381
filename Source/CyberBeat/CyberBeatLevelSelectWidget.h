// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CyberBeatLevelSelectWidget.generated.h"

/**
 * 
 */

class UButton;
UCLASS()
class CYBERBEAT_API UCyberBeatLevelSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	//initialize UI
	virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Level1Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Level2Button;

    // Button click functions
    UFUNCTION()
    void LoadLevel1();

    UFUNCTION()
    void LoadLevel2();




};
