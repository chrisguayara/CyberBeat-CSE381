// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatLevelSelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UCyberBeatLevelSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Bind button click events
	if (Level1Button)
	{
		Level1Button->OnClicked.AddDynamic(this, &UCyberBeatLevelSelectWidget::LoadLevel1);
	}
	if (Level2Button)
	{
		Level2Button->OnClicked.AddDynamic(this, &UCyberBeatLevelSelectWidget::LoadLevel2);
	}
}

void UCyberBeatLevelSelectWidget::LoadLevel1()
{

	UGameplayStatics::OpenLevel(this, FName("Lvl_TopDown"));
}

void UCyberBeatLevelSelectWidget::LoadLevel2()
{

	UGameplayStatics::OpenLevel(this, FName("Lvl_TopDown2"));
}