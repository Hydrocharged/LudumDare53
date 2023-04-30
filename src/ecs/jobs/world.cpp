// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/jobs/world.hpp>
#include <data/data.hpp>

ecs::jobs::World::World() {
	auto& world = ecs::Entity::World();
	world.set(ecs::jobs::World::Singleton{
		.DeltaTime = 0.0,
	});
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
		// Check if a new day has passed, and bring up the perk menu if so
		if (glm::floor(data::world::LastDayTime / HOURS_24F) < glm::floor(data::world::DayTime / HOURS_24F)) {
			//data::world::IsMenuUp = true;
			//TODO: bring up the perk menu

			// Update the gas price
			data::world::GasPrice += 0.10f;

			// Deduct salaries from the player
			auto employeeFilter = Entity::World().filter<const ecs::components::Employee&>();
			employeeFilter.each([](const ecs::components::Employee& employee) {
				data::player::Money -= double(employee.Salary) / 365.0;
			});

			// Add a static amount of money that is generated from all owned buildings
			data::player::Money += float(data::player::OwnedBuildings.size()) * 1000.0f;

			// Game is over when the player runs out of money
			if (data::player::Money <= 0.0f) {
				//TODO: display a proper message and allow restarting the game
				engine::log::Error("You've run out of money! Game Over");
				exit(0);
			}
		}
	}
}
