// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CyberBeatPauseMenu.generated.h"


class UButton;
/**
 * 
 */
UCLASS()
class CYBERBEAT_API UCyberBeatPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void Resume();

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> ResumeButton;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> RestartButton;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> MenuButton;

    /** Name of your main menu level. */
    UPROPERTY(EditDefaultsOnly, Category = "Pause")
    FName MainMenuLevelName = TEXT("/Game/User_Interface/MainMenu");

    UFUNCTION() void OnResume();
    UFUNCTION() void OnRestart();
    UFUNCTION() void OnMenu();
};
