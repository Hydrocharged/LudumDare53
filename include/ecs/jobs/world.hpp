// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_JOBS_WORLD_HPP
#define ECS_JOBS_WORLD_HPP

#include <ecs/ecs.hpp>

namespace ecs::jobs {
	struct World : ecs::Job {
	public:
		struct Singleton {
			double DeltaTime = 0.0;
		};

		World();
		~World();

		void SetDeltaTime(double deltaTime);

	private:
		static void run(const Singleton& s);
		flecs::system system;
	};
}

#endif //ECS_JOBS_WORLD_HPP
