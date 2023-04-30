// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/world.hpp>
#include <data/data.hpp>

ecs::jobs::World::World() {
	auto& world = ecs::Entity::World();
	world.add<ecs::jobs::World::Singleton>();
	system = world.system<const ecs::jobs::World::Singleton&>("World").each(run);
}

ecs::jobs::World::~World() {
	system.destruct();
	ecs::Entity::World().remove<ecs::jobs::World::Singleton>();
}

void ecs::jobs::World::SetDeltaTime(double deltaTime) {
	ecs::Entity::World().get_mut<ecs::jobs::World::Singleton>()->DeltaTime = deltaTime;
}

void ecs::jobs::World::run(const ecs::jobs::World::Singleton& s) {
	data::world::LastDayTime = data::world::DayTime;
	data::world::DayTime += (data::world::IsMenuUp) ? 0.0 : s.DeltaTime * data::world::GameSpeed;
	if (data::world::LastDayTime != data::world::DayTime) {
		//TODO: the rest of the world simulation
	}
}
