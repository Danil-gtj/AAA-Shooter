#include "Actors/Navigation/PatrollingPath.h"

const TArray<FVector>& APatrollingPath::GetWaypoints() const
{
	return WayPoints;
}
