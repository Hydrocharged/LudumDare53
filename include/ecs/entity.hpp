// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_ENTITY_HPP
#define ECS_ENTITY_HPP

#include <engine/application.hpp>
#include <ecs/component.hpp>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>

namespace ecs {
	class Component;

	class Entity {
	public:
		Entity();
		~Entity();

		size_t NumberOfComponents();

		template<class T, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		T* CreateComponent(Args&& ... args);

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		bool HasComponent();

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		T* GetComponent();

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		void RemoveComponent();

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		static void IterateAll(typename std::unordered_set<T*>::iterator& iter, typename std::unordered_set<T*>::iterator& end);

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		static size_t CountAll();

	private:
		static std::unordered_map<size_t, void*>* globalComponentMap();
		static std::unordered_map<size_t, std::function<void(void*)>>* globalComponentRemovalMap();

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		static void componentRemoval(void* component);

		std::unordered_map<size_t, ecs::Component*> components;
	};

	template<class T, class ... Args, typename>
	T* ecs::Entity::CreateComponent(Args&& ... args) {
		// Allocate the component's memory
		void* componentMemory = operator new(sizeof(T));
		auto hash = typeid(T).hash_code();
		// Assign the entity and hash in case the component's constructor wants to use these.
		// This will be compiled away for types that have a default constructor.
		if (!std::is_default_constructible_v<T> && !std::is_trivially_default_constructible_v<T>) {
			reinterpret_cast<T*>(componentMemory)->owner = this;
			reinterpret_cast<T*>(componentMemory)->typeHash = hash;
		}

		// Call the constructor on the memory location to create the component
		T* component = new(componentMemory) T(args...);
		if (components.find(hash) != components.end()) {
			engine::log::Error("Component has already been created");
			return nullptr;
		}
		// Assign/reassign the entity and hash
		component->owner = this;
		component->typeHash = hash;
		components.insert({hash, component});

		auto gComponentMap = globalComponentMap();
		auto gMapIter = gComponentMap->find(hash);
		if (gMapIter == gComponentMap->end()) [[unlikely]] {
			auto set = new std::unordered_set<T*>();
			static_assert(sizeof(std::unordered_set<T*>*) == sizeof(void*));
			gComponentMap->insert({hash, reinterpret_cast<void*>(set)});
			gMapIter = gComponentMap->find(hash);
			if (gMapIter == gComponentMap->end()) [[unlikely]] {
				engine::log::Fatal("Unable to create global component set");
			}

			auto gComponentRemovalMap = globalComponentRemovalMap();
			gComponentRemovalMap->insert({hash, componentRemoval < T > });
		}
		auto set = reinterpret_cast<std::unordered_set<T*>*>(gMapIter->second);
		set->insert(component);

		return component;
	}

	template<class T, typename>
	bool ecs::Entity::HasComponent() {
		auto hash = typeid(T).hash_code();
		return components.find(hash) != components.end();
	}

	template<class T, typename>
	T* ecs::Entity::GetComponent() {
		auto hash = typeid(T).hash_code();
		auto component = components.find(hash);
		if (component != components.end()) {
			return static_cast<T*>(component->second);
		}
		return nullptr;
	}

	template<class T, typename>
	void ecs::Entity::RemoveComponent() {
		auto hash = typeid(T).hash_code();
		auto componentIter = components.find(hash);
		if (componentIter == components.end()) {
			engine::log::Error("Component has already been removed");
			return;
		}
		auto component = componentIter->second;
		components.erase(componentIter);

		auto gComponentMap = globalComponentMap();
		auto gMapIter = gComponentMap->find(hash);
		if (gMapIter != gComponentMap->end()) [[likely]] {
			auto set = reinterpret_cast<std::unordered_set<T*>*>(gMapIter->second);
			set->erase(static_cast<T*>(component));
		} else {
			engine::log::Fatal("Unable to find component set");
		}

		delete component;
	}

	template<class T, typename>
	void ecs::Entity::IterateAll(typename std::unordered_set<T*>::iterator& iter, typename std::unordered_set<T*>::iterator& end) {
		auto hash = typeid(T).hash_code();
		auto gComponentMap = globalComponentMap();
		auto gMapIter = gComponentMap->find(hash);
		if (gMapIter != gComponentMap->end()) {
			auto set = reinterpret_cast<std::unordered_set<T*>*>(gMapIter->second);
			iter = set->begin();
			end = set->end();
		} else {
			typename std::unordered_set<T*>::iterator valIter{};
			iter = valIter;
			end = valIter;
		}
	}

	template<class T, typename>
	size_t ecs::Entity::CountAll() {
		auto hash = typeid(T).hash_code();
		auto gComponentMap = globalComponentMap();
		auto gMapIter = gComponentMap->find(hash);
		if (gMapIter != gComponentMap->end()) [[likely]] {
			auto set = reinterpret_cast<std::unordered_set<T*>*>(gMapIter->second);
			return set->size();
		} else {
			return 0;
		}
	}

	template<class T, typename>
	void ecs::Entity::componentRemoval(void* component) {
		auto hash = typeid(T).hash_code();
		auto gComponentMap = globalComponentMap();
		auto gMapIter = gComponentMap->find(hash);
		if (gMapIter == gComponentMap->end()) [[unlikely]] {
			engine::log::Fatal("Component removal unable to find component set");
		}
		auto set = reinterpret_cast<std::unordered_set<T*>*>(gMapIter->second);
		static_assert(sizeof(ecs::Component*) == sizeof(void*));
		auto derivedComponent = static_cast<T*>(reinterpret_cast<ecs::Component*>(component));
		set->erase(derivedComponent);
	}
}

#endif //ECS_ENTITY_HPP
