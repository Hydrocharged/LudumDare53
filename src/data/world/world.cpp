// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <data/world/initialize.hpp>

namespace data::world {
	std::vector<Node> Nodes{}; // 1 unit == 6 feet
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
	route.EstimatedTimeToCompletion = 0.0f;
	route.TotalLengthEstimate = 0;
	for (const auto& section: sections) {
		auto* road = &data::world::Roads[section.RoadID];
		auto distance = road->Distance();
		route.EstimatedTimeToCompletion += distance / (road->SpeedLimit * MPH_TO_UNITS_PER_SECONDF);
		route.TotalLengthEstimate += std::uint16_t(distance);
	}
	return route;
}

union RouteKey {
	std::uint16_t Buildings[2];
	std::uint32_t Key;
};

bool data::world::Initialize(engine::Application* application) {
	//TODO: read these from a file
	Nodes = {{0,   67},
			 {67,  0},
			 {133, 0},
			 {200, 67},
			 {0,   183},
			 {67,  250},
			 {133, 250},
			 {200, 183},
			 {67,  67},
			 {133, 67},
			 {67,  183},
			 {133, 183}};

	Roads.emplace_back(0, 8, 50);
	Roads.emplace_back(8, 9, 50);
	Roads.emplace_back(9, 3, 50);
	Roads.emplace_back(4, 10, 60);
	Roads.emplace_back(10, 11, 60);
	Roads.emplace_back(11, 7, 60);
	Roads.emplace_back(1, 8, 40);
	Roads.emplace_back(8, 10, 40);
	Roads.emplace_back(10, 5, 40);
	Roads.emplace_back(2, 9, 45);
	Roads.emplace_back(9, 11, 45);
	Roads.emplace_back(11, 6, 45);

	//TODO: When loading in routes, we also need to add the inverse with flipped starts as the file only has one direction
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
