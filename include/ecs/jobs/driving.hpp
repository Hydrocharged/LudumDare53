// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_JOBS_DRIVING_HPP
#define ECS_JOBS_DRIVING_HPP

#include <ecs/ecs.hpp>
#include <ecs/components/human.hpp>
#include <ecs/components/driving.hpp>

namespace ecs::jobs {
	struct Driving : ecs::Job {
	public:
		Driving();
		~Driving();

	private:
		static void run(flecs::entity e, ecs::components::Human& human, ecs::components::Driving& driving);
		flecs::system system;
	};
}

#endif //ECS_JOBS_DRIVING_HPP
