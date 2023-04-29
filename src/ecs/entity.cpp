// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ecs/entity.hpp>

static std::unordered_map<size_t, void*> gComponentMap{};
static std::unordered_map<size_t, std::function<void(void*)>> gComponentRemovalMap{};

ecs::Entity::Entity() = default;

ecs::Entity::~Entity() {
	for (const auto& componentPair: components) {
		auto component = componentPair.second;
		auto removalIter = gComponentRemovalMap.find(component->Type());
		if (removalIter != gComponentRemovalMap.end()) [[likely]] {
			removalIter->second(component);
		} else {
			engine::log::Fatal("Unable to find component removal");
		}
		delete component;
	}
}

size_t ecs::Entity::NumberOfComponents() {
	return components.size();
}

std::unordered_map<size_t, void*>* ecs::Entity::globalComponentMap() {
	return &::gComponentMap;
}

std::unordered_map<size_t, std::function<void(void*)>>* ecs::Entity::globalComponentRemovalMap() {
	return &::gComponentRemovalMap;
}
