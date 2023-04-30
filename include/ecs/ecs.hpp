// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_ECS_HPP
#define ECS_ECS_HPP

#include <engine/application.hpp>
#include <flecs.h>

namespace ecs {
	struct Component {
	};

	class Job {
	public:
		static void SetApplication(engine::Application* application);

	protected:
		static engine::Application* application();
	};

	template<class ... T>
	class Query {
	public:
		Query(Query& query) : q(query) {}
		Query(Query&&) noexcept = default;
		~Query() { q.destruct(); }

		template<typename Func>
		void ForEach(Func&& func) { q.each(func); }

	private:
		Query() = default;

		friend class Entity;

		flecs::query<T...> q;
	};

	class Entity {
	public:
		Entity();
		~Entity();

		template<class T, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		void CreateComponent(Args&& ... args);

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		void CreateComponent(T component);

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		bool HasComponent();

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		const T* GetComponent();

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		T* ModifyComponent();

		template<class T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		void RemoveComponent();

		template<class ... T, typename = std::enable_if_t<std::is_base_of_v<Component, T...>>>
		static std::unique_ptr<ecs::Query<T...>> Query();

		//TODO: hide this and expose all functionality through template functions within this file
		static flecs::world& World();

	private:
		flecs::entity entity;
	};

	template<class T, class ... Args, typename>
	void ecs::Entity::CreateComponent(Args&& ... args) {
		T component{args...};
		entity.set(component);
	}

	template<class T, typename>
	void ecs::Entity::CreateComponent(T component) {
		entity.set(component);
	}

	template<class T, typename>
	bool ecs::Entity::HasComponent() {
		return entity.get<T>() != nullptr;
	}

	template<class T, typename>
	const T* ecs::Entity::GetComponent() {
		return entity.get<T>();
	}

	template<class T, typename>
	T* ecs::Entity::ModifyComponent() {
		return entity.get_mut<T>();
	}

	template<class T, typename>
	void ecs::Entity::RemoveComponent() {
		entity.remove<T>();
	}

	template<class ... T, typename>
	std::unique_ptr<ecs::Query<T...>> ecs::Entity::Query() {
		std::unique_ptr<ecs::Query<T...>> q;
		q.reset(new ecs::Query<T...>());
		q->q = World().query<T...>();
		return std::move(q);
	}
}

#endif //ECS_ECS_HPP
