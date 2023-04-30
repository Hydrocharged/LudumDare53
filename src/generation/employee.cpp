// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <generation/employee.hpp>
#include <data/data.hpp>
#include <ecs/ecs.hpp>
#include <ecs/components/human.hpp>
#include <ecs/components/employee.hpp>
#include <ldutils/random.hpp>

void generation::CreateEmployee(std::uint16_t amount) {
	for (std::uint16_t num = 0; num < amount; ++num) {
		flecs::entity employee = ecs::Entity::World().entity();
		//TODO: should be normal distributions
		ecs::components::Human hComponent{
			.HomeBuilding = 2,
			.CurrentBuilding = 2,
			.WakeupTime = HOURS_24F * ldutils::Randomf(),
			.IsAsleep = false,
		};
		ecs::components::Employee eComponent{
			.Star = std::uint8_t(glm::round(ldutils::Randomf())),
			.Salary = ldutils::Randomf() * 100000.0f,
			.CarEfficiency = (ldutils::Randomf() * 0.5f) + 0.75f,
			.Level = 1,
			.Experience = 0,
			.Speed = (ldutils::Randomf() * 0.6f) + 0.7f,
			.Friendliness = (ldutils::Randomf() * 1.5f) + 0.5f,
			.AccidentLikelihood = ldutils::Randomf() / HOURS_24F,
			.AffectedByWeatherness = ldutils::Randomf() * 2.0f,
			.AverageCustomerSatisfaction = 0.0f,
			.AverageDeliverySpeed = 0.0f,
			.NumberOfDeliveries = 0,
			.LifetimeTips = 0.0f,
		};
		employee.set(hComponent);
		employee.set(eComponent);
	}
}
