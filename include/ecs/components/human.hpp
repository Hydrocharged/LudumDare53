// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_COMPONENTS_HUMAN_HPP
#define ECS_COMPONENTS_HUMAN_HPP

#include <ecs/ecs.hpp>
#include <data/data.hpp>

namespace ecs::components {
	struct Human : ecs::Component {
		std::uint16_t HomeBuilding; // Building Index for Home
		std::uint16_t CurrentBuilding; // Building Index for current location
		float WakeupTime;
		bool IsAsleep;

		float SleepTime() const { return (WakeupTime < HOURS_8F) ? (WakeupTime + HOURS_16F) : (WakeupTime - HOURS_8F); }
	};
}

#endif //ECS_COMPONENTS_HUMAN_HPP
