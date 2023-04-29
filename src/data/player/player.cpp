// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <data/player/initialize.hpp>

namespace data::player {
	double Money = 1000.0;
	std::vector<Perk> Perks{};
	float DeliveryFee = 3.0f;
	float DriverProfitSplit = 0.5f; // This much goes to the driver, the rest goes to the company
	float CompanyDrivingPressure = 0.0f; // 0 is enforce slow driving, 0.5 is relaxed/no pressure, 1 is enforce fast driving
	std::vector<std::uint16_t> OwnedBuildings{}; // Building Indexes
	std::atomic<std::uint64_t> DeliveryID;
}

bool data::player::Initialize(engine::Application* application) {
	//TODO: randomly determine a building that will be our starting one
	DeliveryID.store(5);
	return true;
}

void data::player::Terminate() {}
