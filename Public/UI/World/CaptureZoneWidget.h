#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CaptureZoneWidget.generated.h"

class UProgressBar;
UCLASS()
class GAMECODE_API UCaptureZoneWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* ZoneProgressBar;

	UPROPERTY(EditAnywhere)
	float CaptureTime;

	UPROPERTY(EditAnywhere)
	float LossTime;

	UFUNCTION(BlueprintCallable)
	float UpdateCaptureZoneProgressBar();

	float Progress = 0.0f;

	bool bCapturedByA = false;
	bool bCapturedByB = false;

	bool NowCapturingByA = false;
	bool NowCapturingByB = false;

};
