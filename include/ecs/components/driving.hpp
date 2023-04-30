// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_COMPONENTS_DRIVING_HPP
#define ECS_COMPONENTS_DRIVING_HPP

#include <ecs/ecs.hpp>

namespace ecs::components {
	struct Driving : ecs::Component {
		std::uint16_t Route; // Route Index
		std::uint16_t CurrentSection; // Route Section Index
		float SectionProgress; // How far along we are on this section
		float Speed; // Multiplier on speed limit, affects how much faster/slower this person drives
		std::uint16_t Destination; // Building Index
	};
}

#endif //ECS_COMPONENTS_DRIVING_HPP
