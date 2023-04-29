// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <data/world/initialize.hpp>

namespace data::world {
	std::vector<Node> Nodes{};
	std::vector<Road> Roads{};
	std::vector<Route> Routes{};
	std::vector<Building> Buildings{};
	std::vector<glm::vec2> BuildingSizes{}; // x == width == parallel to vector, y == depth == perpendicular to vector
	std::uint32_t DayNumber = 0;
	float TimeInDay = 0; // Seconds, 86400 seconds in a day
	float GasPrice = 3.00f;
	float GameSpeed = 1.0f;
	WeatherType Weather = WeatherType::Sunny;
}

bool data::world::Initialize(engine::Application* application) {
	//TODO: initialize nodes, then roads, then routes
	//TODO: initialize buildings
	return true;
}

void data::world::Terminate() {}
