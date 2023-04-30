// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef DATA_PLAYER_PLAYER_HPP
#define DATA_PLAYER_PLAYER_HPP

#include <engine/application.hpp>
#include <atomic>

namespace data::player {
	enum class PerkType : std::uint16_t {
		SafeDriving,
		//TODO: think of more perks
	};

	struct Perk {
		PerkType Type;
		float Value; // The strength of the perk, actual effect depends on the perk
	};

	extern double Money;
	extern std::vector<Perk> Perks;
	extern float DeliveryFee;
	extern float DriverProfitSplit; // This much goes to the driver, the rest goes to the company
	extern float CompanyDrivingPressure; // 0 is enforce slow driving, 0.5 is relaxed/no pressure, 1 is enforce fast driving
	extern std::vector<std::uint16_t> OwnedBuildings; // Building Indexes
}

#endif //DATA_PLAYER_PLAYER_HPP
