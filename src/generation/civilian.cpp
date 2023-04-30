// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <generation/civilian.hpp>
#include <data/data.hpp>
#include <ecs/ecs.hpp>
#include <ecs/components/human.hpp>
#include <ecs/components/civilian.hpp>
#include <ldutils/random.hpp>

void generation::CreateCivilian(std::uint16_t amount) {
	for (std::uint16_t num = 0; num < amount; ++num) {
		flecs::entity civilian = ecs::Entity::World().entity();
		//TODO: should be normal distributions
		ecs::components::Human hComponent{
			.HomeBuilding = 2,
			.CurrentBuilding = 2,
			.WakeupTime = HOURS_24F * ldutils::Randomf(),
			.IsAsleep = false,
		};
		ecs::components::Civilian cComponent{
			.OrderRate = (ldutils::Randomf() * 0.6f) + 0.4f,
			.JobLocation = 1,
			.Metabolism = ((ldutils::Randomf() * 0.5f) + 0.5f) / HOURS_8F,
			.Hunger = ldutils::Randomf(),
			.Mood = 1.0f,
			.TipLikelihood = ldutils::Randomf(),
			.TipPercentage = ldutils::Randomf() * 0.3f,
			.IsWaitingOnFood = false,
			.WaitDayTime = 0.0f,
		};
		civilian.set(hComponent);
		civilian.set(cComponent);
	}
}
