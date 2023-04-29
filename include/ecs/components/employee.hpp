// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_COMPONENTS_EMPLOYEE_HPP
#define ECS_COMPONENTS_EMPLOYEE_HPP

#include <ecs/ecs.hpp>

namespace ecs::components {
	struct Employee : ecs::Component {
		std::uint8_t Star; // 1 to 5, higher stars have better stats relative to their salary
		float Salary; // Per year in dollars, deducted daily using (Salary / 365.0f)
		float CarEfficiency; // Multiplier on gas price
		std::uint16_t Level; // Affects driving speed, accident likelihood, satisfaction, etc.
		float Experience; // How much money the driver makes is their experience
		float Speed; // Multiplier, set when creating the driving component
		float Friendliness; // Tip likelihood, amount, and customer satisfaction multiplier
		float AverageCustomerSatisfaction; // 5 out of 5 is perfect
		float AverageDeliverySpeed; // This is relative to the estimated time of arrival without traffic
		std::uint32_t NumberOfDeliveries;
		float LifetimeTips;
	};
}

#endif //ECS_COMPONENTS_EMPLOYEE_HPP