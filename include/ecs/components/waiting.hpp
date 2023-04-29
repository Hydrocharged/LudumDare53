// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_COMPONENTS_WAITING_HPP
#define ECS_COMPONENTS_WAITING_HPP

#include <ecs/ecs.hpp>

namespace ecs::components {
	struct Waiting : ecs::Component {
		std::uint64_t DeliveryID; // Related to data::player::DeliveryID
	};
}

#endif //ECS_COMPONENTS_WAITING_HPP
