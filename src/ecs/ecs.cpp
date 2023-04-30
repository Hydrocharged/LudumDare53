// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/ecs.hpp>

static flecs::world world{};
static engine::Application* gApplication = nullptr;

// Job -----------------------------------------------------------------------------------------------------------------

engine::Application* ecs::Job::application() {
	return gApplication;
}

void ecs::Job::SetApplication(engine::Application* application) {
	gApplication = application;
}

// Entity --------------------------------------------------------------------------------------------------------------

ecs::Entity::Entity() {
	entity = world.entity();
}

ecs::Entity::~Entity() {
	entity.destruct();
}

flecs::world& ecs::Entity::World() {
	return ::world;
}
