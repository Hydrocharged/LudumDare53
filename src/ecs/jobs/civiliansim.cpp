// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/civiliansim.hpp>
#include <data/data.hpp>
#include <ecs/components/confirmed.hpp>
#include <ecs/components/driving.hpp>
#include <ecs/components/waiting.hpp>
#include <ldutils/random.hpp>

ecs::jobs::CivilianSim::CivilianSim() {
	system = ecs::Entity::World()
		.system<ecs::components::Human&, ecs::components::Civilian&>("CivilianSim")
		.term<ecs::components::Driving>().oper(flecs::Not)
		.each(run);
}

ecs::jobs::CivilianSim::~CivilianSim() {
	system.destruct();
}

void ecs::jobs::CivilianSim::run(flecs::entity e, ecs::components::Human& human, ecs::components::Civilian& civilian) {
	// We do not run if we are paused
	if (data::world::LastDayTime == data::world::DayTime) {
		return;
	}
	auto simulationDeltaTime = data::world::DayTime - data::world::LastDayTime;
	auto timeInCurrentDay = float(data::world::DayTime - data::world::CurrentDayAt12AM());

	// Check if we're asleep
	bool shouldBeAwake = data::world::WithinRange(timeInCurrentDay, human.WakeupTime, human.SleepTime());
	if (human.IsAsleep) {
		// Check if we should wake up. If not, then we return.
		if (shouldBeAwake) {
			human.IsAsleep = false;
			civilian.Mood = 1.0f;
		} else {
			return;
		}
	}

	// Check if we're waiting on food
	if (e.has<ecs::components::Waiting>()) {
		bool confirmed = e.has<ecs::components::Confirmed>();
		auto timeWaiting = float(data::world::DayTime - civilian.WaitDayTime);
		// If we're waiting and our order hasn't been confirmed yet, then check how long it's been.
		// If it's been two hours or more, then we cancel, get a refund, and lower our chances of ordering again.
		if (!confirmed && timeWaiting >= HOURS_2F) {
			civilian.OrderRate -= 0.6f;
			e.remove<ecs::components::Waiting>();
			// We never got our food, so we're still hungry.
			civilian.Hunger = 0.0f;
			//TODO: display message that a customer has cancelled their order for waiting too long
		} else if (!confirmed && timeWaiting > HOURS_1F) {
			// It's been less than two hours, but over one hour, so our experience makes us less likely to order again
			// as we continue to wait.
			civilian.OrderRate -= float(simulationDeltaTime / HOURS_5);
		}
	}

	// Check if we're home
	if (human.CurrentBuilding == human.HomeBuilding) {
		// If we are awake and we should not be, then we go to sleep
		if (!shouldBeAwake) {
			human.IsAsleep = true;
			return;
		}
		// If it is time for work and we're not at work, then go to work
		if (data::world::WithinRange(timeInCurrentDay,
									 data::world::ConstrainTo24Hours(human.WakeupTime + HOURS_1F),
									 data::world::ConstrainTo24Hours(human.WakeupTime + HOURS_10F))) {
			bool sameRoad;
			auto routeID = data::world::GetRouteID(human.CurrentBuilding, civilian.JobLocation, sameRoad);
			// If we work on the same section of road, then assume we walked there and teleport us
			if (sameRoad) {
				human.CurrentBuilding = civilian.JobLocation;
			} else {
				// We don't work on the same section of road, so we must drive there
				const auto& currentBuilding = data::world::Buildings[human.CurrentBuilding];
				auto driving = ecs::components::Driving{
					.Route = routeID,
					.CurrentSection = 0,
					.SectionProgress = (data::world::Routes[routeID].Sections[0].Start == currentBuilding.Start) ?
									   currentBuilding.DistanceFromStart :
									   1.0f - currentBuilding.DistanceFromStart,
					.Speed = (ldutils::Randomf() * 0.5f) + 0.75f,
					.Destination = civilian.JobLocation,
				};
				e.set(driving);
			}
			return;
		}
		civilian.Mood += float(simulationDeltaTime * ldutils::Random() * (1.0 / HOURS_5));
	}

	// Check if we're at work
	if (human.CurrentBuilding == civilian.JobLocation) {
		// Check if work is over and we need to head home
		if (data::world::WithinRange(timeInCurrentDay,
									 data::world::ConstrainTo24Hours(human.WakeupTime + HOURS_10F),
									 human.SleepTime())) {
			bool sameRoad;
			auto routeID = data::world::GetRouteID(human.CurrentBuilding, human.HomeBuilding, sameRoad);
			// If we live on the same section of road, then assume we walked there and teleport us
			if (sameRoad) {
				human.CurrentBuilding = human.HomeBuilding;
			} else {
				// We don't live on the same section of road, so we must drive there
				const auto& currentBuilding = data::world::Buildings[human.CurrentBuilding];
				auto driving = ecs::components::Driving{
					.Route = routeID,
					.CurrentSection = 0,
					.SectionProgress = (data::world::Routes[routeID].Sections[0].Start == currentBuilding.Start) ?
									   currentBuilding.DistanceFromStart :
									   1.0f - currentBuilding.DistanceFromStart,
					.Speed = (ldutils::Randomf() * 0.5f) + 0.75f,
					.Destination = human.HomeBuilding,
				};
				e.set(driving);
			}
			return;
		}
		civilian.Mood -= float(simulationDeltaTime * ldutils::Random() * (1.0 / HOURS_10));
	}

	civilian.Hunger -= civilian.Metabolism * float(simulationDeltaTime);
	if (civilian.Hunger <= 0.0f) {
		civilian.Hunger = 1.0f;
		if (ldutils::Randomf() <= civilian.OrderRate + (float(data::player::OwnedBuildings.size()) / 300.0f) + ((3.0f - data::player::DeliveryFee) / 10.0f)) {
			// We decided to order from De-Liver!
			e.add<ecs::components::Waiting>();
			civilian.WaitDayTime = data::world::DayTime;
		}
		return;
	}
	//TODO: BONUS: If we have advertising campaigns, then we should have order rate slowly decay over time so that ads will increase it
}
