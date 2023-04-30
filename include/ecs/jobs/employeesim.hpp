// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_JOBS_EMPLOYEESIM_HPP
#define ECS_JOBS_EMPLOYEESIM_HPP

#include <ecs/ecs.hpp>
#include <ecs/components/human.hpp>
#include <ecs/components/employee.hpp>

namespace ecs::jobs {
	struct EmployeeSim : ecs::Job {
	public:
		EmployeeSim();
		~EmployeeSim();

	private:
		static void run(flecs::entity e, ecs::components::Human& human, ecs::components::Employee& employee);
		flecs::system system;
	};
}

#endif //ECS_JOBS_EMPLOYEESIM_HPP
