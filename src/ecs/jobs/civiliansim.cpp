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
	//TODO: all the stuff
}
