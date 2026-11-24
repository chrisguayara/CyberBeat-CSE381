#include "CyberBeatCountdownWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UCyberBeatCountdownWidget::Pop(const FText& Text)
{
    if (CountText) { CountText->SetText(Text); }
    PopTime = 0.f;
}

void UCyberBeatCountdownWidget::ShowNumber(int32 N)
{
    Pop(FText::AsNumber(N));
}

void UCyberBeatCountdownWidget::ShowGo()
{
    Pop(FText::FromString(TEXT("GO!")));
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().SetTimer(HideHandle, this, &UCyberBeatCountdownWidget::HideSelf, GoHoldSeconds, false);
    }
}

void UCyberBeatCountdownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (!CountText) return;

    PopTime += InDeltaTime;
    const float A = FMath::Clamp(PopTime / FMath::Max(PopDuration, 0.01f), 0.f, 1.f);
    const float Scale = FMath::InterpEaseOut(PopStartScale, 1.f, A, 3.f);
    CountText->SetRenderScale(FVector2D(Scale, Scale));
    CountText->SetRenderOpacity(FMath::Clamp(A * 4.f, 0.f, 1.f));
}