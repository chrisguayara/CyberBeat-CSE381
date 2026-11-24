// .h
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CyberBeatCountdownWidget.generated.h"

class UTextBlock;
class USoundBase;

UCLASS()
class UCyberBeatCountdownWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void ShowNumber(int32 N);
    void ShowGo();

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> CountText;

    UPROPERTY(EditDefaultsOnly, Category = "Countdown") float PopDuration = 0.35f;
    UPROPERTY(EditDefaultsOnly, Category = "Countdown") float PopStartScale = 1.8f;
    UPROPERTY(EditDefaultsOnly, Category = "Countdown") float GoHoldSeconds = 0.6f;

    UPROPERTY(EditDefaultsOnly, Category = "Countdown|Audio") TObjectPtr<USoundBase> NumberSound;
    UPROPERTY(EditDefaultsOnly, Category = "Countdown|Audio") TObjectPtr<USoundBase> GoSound;

private:
    void Pop(const FText& Text);
    void HideSelf() { RemoveFromParent(); }

    float PopTime = 999.f;
    FTimerHandle HideHandle;
};