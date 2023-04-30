// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/driving.hpp>
#include <data/data.hpp>
#include <ecs/components/civilian.hpp>
#include <ecs/components/employee.hpp>
#include <ldutils/random.hpp>

namespace ecs::jobs {
	flecs::entity DeliveringTo;
}

ecs::jobs::Driving::Driving() {
	auto& world = ecs::Entity::World();
	// We should only be creating a single Driving job, so this should be fine
	DeliveringTo = world.entity();
	system = world.system<flecs::entity, ecs::components::Human&, ecs::components::Driving&>("Driving").each(run);
}

ecs::jobs::Driving::~Driving() {
	DeliveringTo.destruct();
	system.destruct();
}

void ecs::jobs::Driving::run(flecs::entity e, ecs::components::Human& human, ecs::components::Driving& driving) {
	// We do not run if we are paused
	if (data::world::LastDayTime == data::world::DayTime) {
		return;
	}
	float simulationDeltaTime = float(data::world::DayTime - data::world::LastDayTime);
	auto& route = data::world::Routes[driving.Route];
	auto& section = route.Sections[driving.CurrentSection];
	auto* road = &data::world::Roads[section.RoadID];

	bool arrivedAtDestination = false;
	while (!arrivedAtDestination && simulationDeltaTime > 0.0f) {
		float trafficCongestionMultiplier = (1.0f / (std::min(20.0f, float(road->CarCount.load())) + 1.0f));
		//TODO: have the company pressure affect their speed
		float currentSpeedMPH = road->SpeedLimit * driving.Speed * trafficCongestionMultiplier;
		float currentSpeedUPS = currentSpeedMPH * MPH_TO_UNITS_PER_SECONDF;

		//TODO: accident likelihood if employee, affected by weather and traffic density (scales by distance traveled)
		float distanceTraveledInDeltaTime = currentSpeedUPS * simulationDeltaTime;
		float roadLength = road->Distance();
		float roadRemaining;
		if (driving.CurrentSection == route.SectionCount - 1) {
			// In the last section, so our roadRemaining is determined by our building location
			const auto& destination = data::world::Buildings[driving.Destination];
			assert(destination.RoadID == section.RoadID);
			float distanceFromStart = (section.Start == destination.Start) ? destination.DistanceFromStart : 1.0f - destination.DistanceFromStart;
			roadRemaining = (roadLength * distanceFromStart) - (roadLength * driving.SectionProgress);
		} else {
			// Not in the last section, so we're traveling the full length of the road regardless of direction
			roadRemaining = roadLength - (roadLength * driving.SectionProgress);
		}

		if (distanceTraveledInDeltaTime >= roadRemaining) {
			road->CarCount.fetch_add(-1);
			simulationDeltaTime *= 1.0f - (roadRemaining / distanceTraveledInDeltaTime);
			driving.CurrentSection += 1;
			// Move to the next section if it exists, else set arrivedAtDestination
			if (driving.CurrentSection < route.SectionCount) {
				// We have more sections, so we add ourselves to the new road and increment the current section
				section = route.Sections[driving.CurrentSection];
				road = &data::world::Roads[section.RoadID];
				road->CarCount.fetch_add(1);
			} else {
				arrivedAtDestination = true;
			}
		} else {
			driving.SectionProgress += roadLength / distanceTraveledInDeltaTime;
			simulationDeltaTime = 0.0f;
		}
	}

	if (arrivedAtDestination) {
		human.CurrentBuilding = driving.Destination;
		e.remove<ecs::components::Driving>();

		flecs::entity civilianEntity = e.target(DeliveringTo);
		if (civilianEntity.is_valid()) {
			// We are delivering to a customer
			e.remove(DeliveringTo, civilianEntity);
			auto* employee = e.get_mut<ecs::components::Employee>();
			auto* civilian = civilianEntity.get_mut<ecs::components::Civilian>();
			float timeWaited = float(data::world::DayTime - double(civilian->WaitDayTime));
			float timeOverEstimate = timeWaited - (route.EstimatedTimeToCompletion * 1.5f); // 50% buffer before penalties
			if (timeOverEstimate > 0.0f) {
				civilian->Mood -= timeOverEstimate / 7200.0f; // Full mood reduction after two hours of over waiting
			}
			if (ldutils::Randomf() < civilian->TipLikelihood * civilian->Mood * employee->Friendliness) {
				float tip = civilian->TipPercentage * civilian->Mood * employee->Friendliness;
				employee->LifetimeTips += tip;
				employee->Experience += tip;
			}
			//TODO: calculate customer satisfaction (timeOverEstimate, friendliness, delivery fee)
			//TODO: add money to the player and the employee from profit split
			//TODO: civilian updates mood, order rate
			//TODO: deduct gas price as a factor of route distance and employee car efficiency
			civilian->Hunger = 1;
			civilian->IsWaitingOnFood = false;
		}
	}
}
