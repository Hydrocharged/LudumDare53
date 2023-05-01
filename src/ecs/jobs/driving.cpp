// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/driving.hpp>
#include <data/data.hpp>
#include <ecs/components/confirmed.hpp>
#include <ecs/components/waiting.hpp>
#include <ldutils/random.hpp>

namespace ecs::jobs {
	flecs::entity DeliveringTo;
}

ecs::jobs::Driving::Driving() {
	auto& world = ecs::Entity::World();
	// We should only be creating a single Driving job, so this should be fine
	DeliveringTo = world.entity();
	system = world.system<ecs::components::Human&, ecs::components::Driving&>("Driving").each(run);
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
		float currentSpeedMPH = road->SpeedLimit * driving.Speed * trafficCongestionMultiplier;
		float currentSpeedUPS = currentSpeedMPH * MPH_TO_UNITS_PER_SECONDF;

		//TODO: accident likelihood if employee, affected by weather and traffic density (scales by distance traveled)
		// components::Waiting.Confirmed = false; e.remove(DeliveringTo, civilianEntity);
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
			civilianEntity.remove<ecs::components::Waiting>();
			civilianEntity.remove<ecs::components::Confirmed>();

			auto* employee = e.get_mut<ecs::components::Employee>();
			auto* civilian = civilianEntity.get_mut<ecs::components::Civilian>();
			ProcessDelivery(route.EstimatedTimeToCompletion, employee, civilian);
		}

		// As an employee, the company pays for ALL gas used
		if (auto employee = e.get<ecs::components::Employee>()) {
			float routeInMiles = float(route.TotalLengthEstimate) * UNITS_TO_MILESF;
			float gallonsUsed = routeInMiles / (30.0f * employee->CarEfficiency);
			data::player::Money -= gallonsUsed * data::world::GasPrice;
		}
	}
}

void ecs::jobs::Driving::ProcessDelivery(float estimatedTimeToCompletion, ecs::components::Employee* employee, ecs::components::Civilian* civilian) {
	float timeWaited = float(data::world::DayTime - civilian->WaitDayTime);
	float timeOverEstimate = timeWaited - (estimatedTimeToCompletion * 1.5f); // 50% buffer before penalties
	if (timeOverEstimate > 0.0f) {
		civilian->Mood -= timeOverEstimate / 7200.0f; // Full mood reduction after two hours of over waiting
	}
	if (ldutils::Randomf() < civilian->TipLikelihood * civilian->Mood * employee->Friendliness) {
		float tip = data::player::OrderPrice * civilian->TipPercentage * civilian->Mood * employee->Friendliness;
		employee->LifetimeTips += tip;
		employee->Experience += tip;
	}

	// Customer satisfaction affected by:
	//   Time under or over buffered estimate. Delivering a full 90 minutes early will give a full rating on its own
	//   Friendliness over 1.0 will give an increase to customer satisfaction, under 1.0 will decrease it
	//   A delivery fee of 3.00 is viewed as fair, so every 1.00 above or under moves the rating by a star
	float customerSatisfaction = std::clamp((-timeOverEstimate / 5400.0f) +
											((employee->Friendliness - 1.0f) * 1.5f) +
											((3.0f - data::player::DeliveryFee) * 0.2f), 0.0f, 1.0f);
	// Order rate moves up 10% for a perfect order, moves down 10% for a tragedy
	civilian->OrderRate += (customerSatisfaction - 0.5f) * 0.1f;
	civilian->Hunger = 1;
	civilian->IsWaitingOnFood = false;

	float totalSale = data::player::OrderPrice + data::player::DeliveryFee;
	employee->Experience += totalSale * data::player::DriverProfitSplit;
	data::player::Money += totalSale * (1.0f - data::player::DriverProfitSplit);
	employee->AverageCustomerSatisfaction = ((float(employee->NumberOfDeliveries) * employee->AverageCustomerSatisfaction) + customerSatisfaction) /
											float(employee->NumberOfDeliveries + 1);
	employee->NumberOfDeliveries += 1;
}
