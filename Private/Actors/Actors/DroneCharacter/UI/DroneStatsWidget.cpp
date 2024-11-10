#include "Actors/Actors/DroneCharacter/UI/DroneStatsWidget.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"

void UDroneStatsWidget::SetYReticlePosition(float PosYValue)
{
	PosY = PosYValue;
}

float UDroneStatsWidget::GetYReticlePosition()
{
	return  PosY * -300.0f;
}

void UDroneStatsWidget::SetXReticlePosition(float PosXValue)
{
	PosX = PosXValue;
}

float UDroneStatsWidget::GetXReticlePosition()
{
	return PosX * 300.0f;
}

void UDroneStatsWidget::SetReticlePositionInViewport()
{
	if (IsValid(ReticleWidget))
	{
		ReticleWidget->SetPositionInViewport(FMath::Lerp(FVector2D(0.0f, 0.0f), FVector2D(GetXReticlePosition(), GetYReticlePosition()), 0.5f));
	}
}