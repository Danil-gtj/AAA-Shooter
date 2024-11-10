#include "Components/CharacterComponents/AIPatrollingComponent.h"
#include "Actors/Navigation/PatrollingPath.h"

bool UAIPatrollingComponent::CanPatrol() const
{
	return IsValid(PatrollingPath) && PatrollingPath->GetWaypoints().Num() > 0;
}

FVector UAIPatrollingComponent::GetClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> WayPoints = PatrollingPath->GetWaypoints();
	FTransform PathTransform = PatrollingPath->GetActorTransform();

	FVector ClosestWayPoint;
	float MinSqDistance = FLT_MAX;
	for (int32 i = 0; i < WayPoints.Num(); i++)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if (CurrentSqDistance < MinSqDistance)
		{
			MinSqDistance = CurrentSqDistance;
			ClosestWayPoint = WayPointWorld;
			CurrentWaypointIndex = i;
		}
	}

	return ClosestWayPoint;
}

FVector UAIPatrollingComponent::SelectNextWaypoint()
{
	//++CurrentWaypointIndex;
	const TArray<FVector> WayPoints = PatrollingPath->GetWaypoints();
	FVector NextWayPoint;

	if (PatrollingType == EPatrollingType::Circle)
	{
		CurrentWaypointIndex++;
		if (CurrentWaypointIndex == PatrollingPath->GetWaypoints().Num())
		{
			CurrentWaypointIndex = 0;
		}
	}
	else if (PatrollingType == EPatrollingType::PingPong)
	{
		if (PingPong) CurrentWaypointIndex++;

		if (CurrentWaypointIndex == PatrollingPath->GetWaypoints().Num() && PingPong)
		{
			PingPong = false;
			CurrentWaypointIndex--;
		}

		if (!PingPong)
		{
			CurrentWaypointIndex--;
			if (CurrentWaypointIndex < 0)
			{
				CurrentWaypointIndex = 0;
				PingPong = true;
			}
		}
	}

	FTransform PathTransform = PatrollingPath->GetActorTransform();

	NextWayPoint = PathTransform.TransformPosition(WayPoints[CurrentWaypointIndex]);

	return NextWayPoint;

}
