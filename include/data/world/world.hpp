// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef DATA_WORLD_WORLD_HPP
#define DATA_WORLD_WORLD_HPP

#include <engine/application.hpp>

namespace data::world {
	struct Road;
	struct RouteSection;
	struct Route;
	struct Building;

	using Node = typename glm::vec2;

	enum class WeatherType : std::uint8_t { // Multiplier == (100 / Weather)
		Sunny = 100,
		Foggy = 90,
		Rainy = 80,
		Snowy = 60,
		Icy = 40,
	};

	extern std::vector<Node> Nodes;
	extern std::vector<Road> Roads;
	extern std::vector<Route> Routes;
	extern std::vector<Building> Buildings;
	extern std::vector<glm::vec2> BuildingSizes; // x == width == parallel to vector, y == depth == perpendicular to vector
	extern double DayTime; // Seconds, 86400 seconds in a day, Day == floor(DayTime / 86400)
	extern double LastDayTime; // Last DayTime, if LastDayTime == DayTime then the game is paused
	extern bool IsMenuUp;
	extern float GasPrice;
	extern double GameSpeed;
	extern WeatherType Weather;

	struct Road {
		std::uint16_t NodeA;
		std::uint16_t NodeB;
		float SpeedLimit;
		std::uint32_t CarCount;

		float Distance() const { return glm::length(Nodes[NodeA] - Nodes[NodeB]); }
	};

	struct RouteSection {
		std::uint16_t RoadID;
		std::uint16_t Start; // Will be 0 or 1, aligned to two-byte boundary
	};

	struct Route {
		RouteSection* Sections;
		float TotalDistance;
		std::uint16_t SectionCount;
	};

	enum class BuildingType : std::uint16_t { // aligned to two-byte boundary
		Home,
		Office,
		Restaurant
	};

	struct Building {
		std::uint16_t RoadID;
		std::uint16_t Start; // Will be 0 or 1, aligned to two-byte boundary
		float DistanceFromStart;
		std::uint16_t Cost; // Maybe in thousands, so 10 == $10,000
		BuildingType Type;
	};

	static std::uint32_t GetRouteID(std::uint16_t buildingStart, std::uint16_t buildingDest);
}

#endif //DATA_WORLD_WORLD_HPP
