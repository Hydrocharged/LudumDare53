// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/driving.hpp>
#include <data/data.hpp>

ecs::jobs::Driving::Driving() {
	system = ecs::Entity::World()
		.system<flecs::entity, ecs::components::Human&, ecs::components::Driving&>("Driving")
		.each(run);
}

ecs::jobs::Driving::~Driving() {
	system.destruct();
}

void ecs::jobs::Driving::run(flecs::entity e, ecs::components::Human& human, ecs::components::Driving& driving) {
	//TODO: all the stuff
}
