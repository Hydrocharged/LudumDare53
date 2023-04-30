// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/employeesim.hpp>
#include <data/data.hpp>
#include <ecs/components/driving.hpp>

ecs::jobs::EmployeeSim::EmployeeSim() {
	system = ecs::Entity::World()
		.system<flecs::entity, ecs::components::Human&, ecs::components::Employee&>("EmployeeSim")
		.term<ecs::components::Driving>().oper(flecs::Not)
		.each(run);
}

ecs::jobs::EmployeeSim::~EmployeeSim() {
	system.destruct();
}

void ecs::jobs::EmployeeSim::run(flecs::entity e, ecs::components::Human& human, ecs::components::Employee& employee) {
	// We do not run if we are paused
	if (data::world::LastDayTime == data::world::DayTime) {
		return;
	}
	//TODO: Am I asleep? Should I wake up? If yes, wake up, if no, do nothing
	//TODO: Am I home and need to sleep? Then go to sleep
	//TODO: Am I home and need to start working? Travel to work
	//TODO: Am I working and done for the day? Then head home
	//TODO: Should I make a delivery? If yes, check if already at a restaurant, else drive to nearest one
	//TODO: Am I waiting at a restaurant? If yes, continue to wait until an order is ready
	//TODO: Drive to the nearest restaurant and wait if nothing else to do
}
