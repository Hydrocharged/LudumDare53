// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/civiliansim.hpp>
#include <data/data.hpp>
#include <ecs/components/driving.hpp>

ecs::jobs::CivilianSim::CivilianSim() {
	system = ecs::Entity::World()
		.system<flecs::entity, ecs::components::Human&, ecs::components::Civilian&>("CivilianSim")
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
	//TODO: Am I asleep? Should I wake up? If yes, wake up, if no, do nothing
	//TODO: Am I waiting on food? If yes, continue waiting
	// (mood/order rate drains if waiting over an hour without confirmation, cancels/gets refund after 2 hours without confirmation and order rate plummets)
	//TODO: Am I home and need to sleep? Then go to sleep
	//TODO: Am I home and need to start working? Travel to work
	//TODO: Am I working and done for the day? Then head home
	//TODO: Deduct metabolism then check if hungry enough to order? If yes, do I order from De-Liver?
	// (order rate determined by variable, number of locations, and delivery fee) | (don't order if on same street as a restaurant)
	//TODO: Drain mood if at work, increase if home
	//TODO: BONUS: If we have advertising campaigns, then we should have order rate slowly decay over time so that ads will increase it
}
