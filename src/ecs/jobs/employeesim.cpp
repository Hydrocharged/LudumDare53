// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/employeesim.hpp>
#include <data/data.hpp>
#include <ecs/components/confirmed.hpp>
#include <ecs/components/driving.hpp>
#include <ecs/components/waiting.hpp>
#include <ecs/jobs/driving.hpp>
#include <ldutils/random.hpp>

namespace ecs::jobs {
	flecs::query<ecs::components::Waiting> WaitingAndUnconfirmed;
}

ecs::jobs::EmployeeSim::EmployeeSim() {
	WaitingAndUnconfirmed = ecs::Entity::World()
		.query_builder<ecs::components::Waiting>()
		.term<ecs::components::Confirmed>().oper(flecs::Not)
		.build();
	system = ecs::Entity::World()
		.system<ecs::components::Human&, ecs::components::Employee&>("EmployeeSim")
		.term<ecs::components::Driving>().oper(flecs::Not)
		.each(run);
}

ecs::jobs::EmployeeSim::~EmployeeSim() {
	system.destruct();
	WaitingAndUnconfirmed.destruct();
}

void ecs::jobs::EmployeeSim::run(flecs::entity e, ecs::components::Human& human, ecs::components::Employee& employee) {
	// We do not run if we are paused
	if (data::world::LastDayTime == data::world::DayTime) {
		return;
	}
	auto simulationDeltaTime = data::world::DayTime - data::world::LastDayTime;
	auto timeInCurrentDay = float(data::world::DayTime - data::world::CurrentDayAt12AM());

	// Check if we need to level up
	while (employee.Experience > 100.0f + (50.0f * float(employee.Level))) {
		employee.Experience -= 100.0f + (50.0f * float(employee.Level));
		employee.Level += 1;
		employee.Speed += float(employee.Star) * (ldutils::Randomf() * 0.03f);
		employee.Friendliness += float(employee.Star) * (ldutils::Randomf() * 0.04f);
		//TODO: figure out the formula for accident likelihood
		//employee.AccidentLikelihood -= float(employee.Star) * (ldutils::Randomf() * 0.001f);
	}

	// Check if we're asleep
	bool shouldBeAwake = data::world::WithinRange(timeInCurrentDay, human.WakeupTime, human.SleepTime());
	if (human.IsAsleep) {
		// Check if we should wake up. If not, then we return.
		if (shouldBeAwake) {
			human.IsAsleep = false;
		} else {
			return;
		}
	}

	// Check if we're home
	if (human.CurrentBuilding == human.HomeBuilding) {
		// If we are awake and we should not be, then we go to sleep
		if (!shouldBeAwake) {
			human.IsAsleep = true;
			return;
		}
		// If it is time for work and we're not at a restaurant, then go to the nearest one
		if (data::world::WithinRange(timeInCurrentDay,
									 data::world::ConstrainTo24Hours(human.WakeupTime + HOURS_1F),
									 data::world::ConstrainTo24Hours(human.WakeupTime + HOURS_10F))) {
			std::uint16_t nearestRestaurant = data::world::GetClosestRestaurant(human.CurrentBuilding);
			bool sameRoad;
			auto routeID = data::world::GetRouteID(human.CurrentBuilding, nearestRestaurant, sameRoad);
			// If we work on the same section of road, then we just teleport there
			if (sameRoad) {
				human.CurrentBuilding = nearestRestaurant;
			} else {
				// We aren't on the same section of road, so we must drive there
				const auto& currentBuilding = data::world::Buildings[human.CurrentBuilding];
				auto driving = ecs::components::Driving{
					.Route = routeID,
					.CurrentSection = 0,
					.SectionProgress = (data::world::Routes[routeID].Sections[0].Start == currentBuilding.Start) ?
									   currentBuilding.DistanceFromStart :
									   1.0f - currentBuilding.DistanceFromStart,
					.Speed = employee.Speed,
					.Destination = nearestRestaurant,
				};
				e.set(driving);
			}
			return;
		}
	}

	// Check if we're not home
	if (human.CurrentBuilding != human.HomeBuilding) {
		// If we're done working for the day, we head home
		if (data::world::WithinRange(timeInCurrentDay,
									 data::world::ConstrainTo24Hours(human.WakeupTime + HOURS_10F),
									 human.SleepTime())) {
			bool sameRoad;
			auto routeID = data::world::GetRouteID(human.CurrentBuilding, human.HomeBuilding, sameRoad);
			// If we live on the same section of road that we're already on, then teleport us
			if (sameRoad) {
				human.CurrentBuilding = human.HomeBuilding;
			} else {
				// We don't live on this road, so we must drive there
				const auto& currentBuilding = data::world::Buildings[human.CurrentBuilding];
				auto driving = ecs::components::Driving{
					.Route = routeID,
					.CurrentSection = 0,
					.SectionProgress = (data::world::Routes[routeID].Sections[0].Start == currentBuilding.Start) ?
									   currentBuilding.DistanceFromStart :
									   1.0f - currentBuilding.DistanceFromStart,
					.Speed = employee.Speed,
					.Destination = human.HomeBuilding,
				};
				e.set(driving);
			}
			return;
		}

		// If we're at a restaurant, then we should check for orders
		if (data::world::Buildings[human.CurrentBuilding].Type == data::world::BuildingType::Restaurant) {
			//TODO: have the company pressure affect their speed

			// Grab the first civilian that is waiting and has not yet had their order confirmed
			flecs::entity waitingCivilian = WaitingAndUnconfirmed.first();
			if (waitingCivilian.is_valid()) {
				auto civilianHuman = waitingCivilian.get<ecs::components::Human>();
				bool sameRoad;
				auto routeID = data::world::GetRouteID(human.CurrentBuilding, civilianHuman->CurrentBuilding, sameRoad);
				// If we work on the same section of road, then we just teleport there
				if (sameRoad) {
					human.CurrentBuilding = civilianHuman->CurrentBuilding;
					waitingCivilian.remove<ecs::components::Waiting>();
					ecs::jobs::Driving::ProcessDelivery(10.0f, &employee, waitingCivilian.get_mut<ecs::components::Civilian>());
				} else {
					// We aren't on the same section of road, so we must drive there
					waitingCivilian.add<ecs::components::Confirmed>();
					e.add(DeliveringTo, waitingCivilian);

					const auto& currentBuilding = data::world::Buildings[human.CurrentBuilding];
					auto driving = ecs::components::Driving{
						.Route = routeID,
						.CurrentSection = 0,
						.SectionProgress = (data::world::Routes[routeID].Sections[0].Start == currentBuilding.Start) ?
										   currentBuilding.DistanceFromStart :
										   1.0f - currentBuilding.DistanceFromStart,
						.Speed = employee.Speed,
						.Destination = civilianHuman->CurrentBuilding,
					};
					e.set(driving);
				}
			}
		} else {
			// Otherwise we head to the nearest restaurant
			std::uint16_t nearestRestaurant = data::world::GetClosestRestaurant(human.CurrentBuilding);
			bool sameRoad;
			auto routeID = data::world::GetRouteID(human.CurrentBuilding, nearestRestaurant, sameRoad);
			// If we work on the same section of road, then we just teleport there
			if (sameRoad) {
				human.CurrentBuilding = nearestRestaurant;
			} else {
				// We aren't on the same section of road, so we must drive there
				const auto& currentBuilding = data::world::Buildings[human.CurrentBuilding];
				auto driving = ecs::components::Driving{
					.Route = routeID,
					.CurrentSection = 0,
					.SectionProgress = (data::world::Routes[routeID].Sections[0].Start == currentBuilding.Start) ?
									   currentBuilding.DistanceFromStart :
									   1.0f - currentBuilding.DistanceFromStart,
					.Speed = employee.Speed,
					.Destination = nearestRestaurant,
				};
				e.set(driving);
			}
			return;
		}
	}
}
