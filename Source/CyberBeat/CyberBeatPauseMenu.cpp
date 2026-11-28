// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatPauseMenu.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "CyberBeatMusicManager.h"
#include "EngineUtils.h"

void UCyberBeatPauseMenu::NativeConstruct()
{
    Super::NativeConstruct();
    ResumeButton->OnClicked.AddDynamic(this, &UCyberBeatPauseMenu::OnResume);
    RestartButton->OnClicked.AddDynamic(this, &UCyberBeatPauseMenu::OnRestart);
    MenuButton->OnClicked.AddDynamic(this, &UCyberBeatPauseMenu::OnMenu);
}


void UCyberBeatPauseMenu::OnRestart()
{
    UGameplayStatics::SetGamePaused(this, false);
    UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void UCyberBeatPauseMenu::OnMenu()
{
    UGameplayStatics::SetGamePaused(this, false);
    UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}

void UCyberBeatPauseMenu::OnResume() { Resume(); }

void UCyberBeatPauseMenu::Resume()
{
    APlayerController* PC = GetOwningPlayer();
    UGameplayStatics::SetGamePaused(this, false);
    for (TActorIterator<ACyberBeatMusicManager> It(GetWorld()); It; ++It) It->ResumeMusic();

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);
    PC->SetShowMouseCursor(false);

    RemoveFromParent();
}