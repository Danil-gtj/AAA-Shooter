#include "UI/World/CaptureZoneWidget.h"
#include "GameStates/CaptureSharpensGameState.h"
#include "Kismet/GameplayStatics.h"

float UCaptureZoneWidget::UpdateCaptureZoneProgressBar()
{
	ACaptureSharpensGameState* GS = Cast<ACaptureSharpensGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS->PlayersAInZone > GS->PlayersBInZone && bCapturedByA == false)
	{
		Progress += FMath::Clamp(GS->GetServerWorldTimeSeconds() * CaptureTime * 0.01f, 0.0f, 1.1f);
		NowCapturingByA = true;
		if (Progress >= 1.0f)
		{
			bCapturedByA = true;
			bCapturedByB = false;
		}
	}
	else if(NowCapturingByB == false && bCapturedByB == false && bCapturedByA == false)
	{
		Progress -= FMath::Clamp(GS->GetServerWorldTimeSeconds() * LossTime * 0.01f, 0.0f, 1.1f);
	}

	if (GS->PlayersBInZone > GS->PlayersAInZone && bCapturedByB == false)
	{
		Progress += FMath::Clamp(GS->GetServerWorldTimeSeconds() * CaptureTime * 0.01f, 0.0f, 1.1f);
		NowCapturingByB = true;
		if (Progress >= 1.0f)
		{
			bCapturedByA = false;
			bCapturedByB = true;
		}
	}
	else if(NowCapturingByA == false && bCapturedByA == false && bCapturedByB == false)
	{
		Progress -= FMath::Clamp(GS->GetServerWorldTimeSeconds() * LossTime * 0.01f, 0.0f, 1.1f);
	}

	return Progress;
}
