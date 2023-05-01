// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_COMPONENTS_CIVILIAN_HPP
#define ECS_COMPONENTS_CIVILIAN_HPP

#include <ecs/ecs.hpp>

namespace ecs::components {
	struct Civilian : ecs::Component {
		float OrderRate; // Likehood to order from our company
		std::uint16_t JobLocation; // Building Index
		float Metabolism; // Rate that hunger depletes when awake
		float Hunger; // When hunger <= 0, then they order food
		float Mood; // Impacts tip likelihood/percentage, randomly decreases at work, increase when home, wakeup with 1.0, affected by delivery fee
		float TipLikelihood; // 0 never tips, 1 always tips
		float TipPercentage; // Of total order
		bool IsWaitingOnFood;
		double WaitDayTime; // Relates to data::world::DayTime
	};
}

#endif //ECS_COMPONENTS_CIVILIAN_HPP
