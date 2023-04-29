// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_COMPONENT_HPP
#define ECS_COMPONENT_HPP

#include <engine/application.hpp>

namespace ecs {
	class Entity;

	class Component {
	public:
		Component();
		virtual ~Component();

		[[nodiscard]] size_t Type() const;
		ecs::Entity* Entity();

	private:
		friend ecs::Entity;

		ecs::Entity* owner;
		size_t typeHash;
	};
}

#endif //ECS_COMPONENT_HPP
