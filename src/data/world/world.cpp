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
	double DayTime = 0.0; // Seconds, 86400 seconds in a day, Day == floor(DayTime / 86400)
	double LastDayTime = 0.0; // Last DayTime, if LastDayTime == DayTime then the game is paused
	bool IsMenuUp = false;
	float GasPrice = 3.00f;
	double GameSpeed = 60.0; // 1 real-world second is 1 in-game minute
	WeatherType Weather = WeatherType::Sunny;

	std::unordered_map<std::uint32_t, std::uint32_t> BuildingsToRouteID{}; // The key == (uint16:BuildingA << 16) | uint16:BuildingB
}

//TODO: DELETE ME
data::world::Route QRoute(std::vector<data::world::RouteSection> sections) {
	data::world::Route route{};
	route.Sections = new data::world::RouteSection[sections.size()];
	memcpy(route.Sections, sections.data(), sizeof(data::world::RouteSection) * sections.size());
	route.SectionCount = sections.size();
	route.TotalDistance = 0.0f;
	for (const auto& section: sections) {
		route.TotalDistance += data::world::Roads[section.RoadID].Distance();
	}
	return route;
}

union RouteKey {
	std::uint16_t Buildings[2];
	std::uint32_t Key;
};

bool data::world::Initialize(engine::Application* application) {
	//TODO: read these from a file
	Nodes = {{0,    400},
			 {400,  0},
			 {800,  0},
			 {1200, 400},
			 {0,    1100},
			 {400,  1500},
			 {800,  1500},
			 {1200, 1100},
			 {400,  400},
			 {800,  400},
			 {400,  1100},
			 {800,  1100}};
	Roads = {{0,  8,  50, 0},
			 {8,  9,  50, 0},
			 {9,  3,  50, 0},
			 {4,  10, 60, 0},
			 {10, 11, 60, 0},
			 {11, 7,  60, 0},
			 {1,  8,  40, 0},
			 {8,  10, 40, 0},
			 {10, 5,  40, 0},
			 {2,  9,  45, 0},
			 {9,  11, 45, 0},
			 {11, 6,  45, 0}};
	Routes = {
		QRoute({{0, 0},
				{7, 0},
				{4, 0}}),
		QRoute({{0, 0},
				{1, 0},
				{9, 1}}),
		QRoute({{4, 1},
				{7, 1},
				{0, 1}}),
		QRoute({{4,  0},
				{10, 1},
				{9,  1}}),
		QRoute({{9, 0},
				{1, 1},
				{0, 1}}),
		QRoute({{9,  0},
				{10, 0},
				{4,  1}}),
	};
	//TODO: dynamically generate buildings
	Buildings = {
		{
			.RoadID = 0,
			.Start = 1,
			.DistanceFromStart = .35f,
			.Cost = 10,
			.Type = BuildingType::Restaurant,
		},
		{
			.RoadID = 4,
			.Start = 0,
			.DistanceFromStart = .65f,
			.Cost = 20,
			.Type = BuildingType::Office,
		},
		{
			.RoadID = 9,
			.Start = 1,
			.DistanceFromStart = .9f,
			.Cost = 0,
			.Type = BuildingType::Home,
		}};
	//TODO: read the building sizes
	BuildingSizes = {{100, 150},
					 {150, 300},
					 {50,  60}};
	//TODO: read these in
	BuildingsToRouteID = {{RouteKey{.Buildings = {0, 1}}.Key, 0},
						  {RouteKey{.Buildings = {0, 2}}.Key, 1},
						  {RouteKey{.Buildings = {1, 0}}.Key, 2},
						  {RouteKey{.Buildings = {1, 2}}.Key, 3},
						  {RouteKey{.Buildings = {2, 0}}.Key, 4},
						  {RouteKey{.Buildings = {2, 1}}.Key, 5}};
	return true;
}

void data::world::Terminate() {}

std::uint32_t data::world::GetRouteID(std::uint16_t buildingStart, std::uint16_t buildingDest) {
	RouteKey key;
	key.Buildings[0] = buildingStart;
	key.Buildings[1] = buildingDest;
	return BuildingsToRouteID.find(key.Key)->second;
}
