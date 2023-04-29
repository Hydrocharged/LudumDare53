// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/component.hpp>

ecs::Component::Component() {}

ecs::Component::~Component() = default;

size_t ecs::Component::Type() const {
	return typeHash;
}

ecs::Entity* ecs::Component::Entity() {
	return owner;
}
