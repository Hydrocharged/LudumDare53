// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_COMPONENTS_HUMAN_HPP
#define ECS_COMPONENTS_HUMAN_HPP

#include <ecs/ecs.hpp>

namespace ecs::components {
	struct Human : ecs::Component {
		std::uint16_t HomeBuilding; // Building Index for Home
		std::uint16_t CurrentBuilding; // Building Index for current location
		bool IsDriving;
		float WakeupTime;
		float AccidentLikelihood; // Multiplier, checked every simulation step, 0 means unable to get into accidents
		float AffectedByWeatherness; // 0 has no additional multiplier, 1.0 is doubly affected, 2.0 is triple, etc.

		float SleepTime() const { return (WakeupTime < 28800.0f) ? (WakeupTime + 57600.0f) : (WakeupTime - 28800.0f); }
	};
}

#endif //ECS_COMPONENTS_HUMAN_HPP
