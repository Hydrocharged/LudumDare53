// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef DATA_WORLD_WORLD_HPP
#define DATA_WORLD_WORLD_HPP

#define HOURS_24 86400.0
#define HOURS_24F 86400.0f
#define HOURS_16 57600.0
#define HOURS_16F 57600.0f
#define HOURS_10 36000.0
#define HOURS_10F 36000.0f
#define HOURS_8 28800.0
#define HOURS_8F 28800.0f
#define HOURS_5 18000.0
#define HOURS_5F 18000.0f
#define HOURS_2 7200.0
#define HOURS_2F 7200.0f
#define HOURS_1 3600.0
#define HOURS_1F 3600.0f

#define MPH_TO_FEET_PER_SECOND 1.466666666667
#define MPH_TO_FEET_PER_SECONDF 1.466666666667f
#define MPH_TO_UNITS_PER_SECOND (MPH_TO_FEET_PER_SECOND / 6.0)
#define MPH_TO_UNITS_PER_SECONDF (MPH_TO_FEET_PER_SECONDF / 6.0f)

#define UNITS_TO_MILES (6.0 / 5280.0)
#define UNITS_TO_MILESF (6.0f / 5280.0f)

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

	extern std::vector<Node> Nodes; // 1 unit == 6 feet
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
		Road(std::uint16_t nodeA, std::uint16_t nodeB, float speedLimit) : NodeA(nodeA), NodeB(nodeB), SpeedLimit(speedLimit) {}
		Road(Road&& r) noexcept : NodeA(r.NodeA), NodeB(r.NodeB), SpeedLimit(r.SpeedLimit), CarCount(r.CarCount.load()) {}

		std::uint16_t NodeA;
		std::uint16_t NodeB;
		float SpeedLimit;
		std::atomic<std::uint32_t> CarCount{0};

		float Distance() const { return glm::length(Nodes[NodeA] - Nodes[NodeB]); }
	};

	struct RouteSection {
		std::uint16_t RoadID;
		std::uint16_t Start; // Will be 0 or 1, aligned to two-byte boundary
	};

	struct Route {
		RouteSection* Sections;
		float EstimatedTimeToCompletion;
		std::uint16_t SectionCount;
		std::uint16_t TotalLengthEstimate; //TODO: might need to divide by some number to make it all fit
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

		glm::vec2 LocationInUnits() const {
			auto& road = Roads[RoadID];
			glm::vec2 nodeA = Nodes[road.NodeA];
			glm::vec2 nodeB = Nodes[road.NodeB];
			return (Start == 0) ? nodeA + ((nodeB - nodeA) * DistanceFromStart) : nodeB + ((nodeA - nodeB) * DistanceFromStart);
		}
	};

	std::uint32_t GetRouteID(std::uint16_t buildingStart, std::uint16_t buildingDest, bool& sameRoad);
	std::uint16_t GetClosestRestaurant(std::uint16_t currentBuilding);
	// This returns the current day at 12AM. In other words, we take the current time (which includes the day), reverse
	// it to 12AM, and return that number. This is useful to determine waking and sleeping hours (by just appending that
	// time to this result).
	double CurrentDayAt12AM();
	// Given a time, makes sure that it is within 12:00AM to 11:59:59PM. Used for offsets, e.g. (4AM - 6 hours) == 10PM
	inline float ConstrainTo24Hours(float time) { return (time >= HOURS_24F) ? time - HOURS_24F : ((time < 0) ? time + HOURS_24F : time); }
	inline bool WithinRange(float currentTime, float rangeStart, float rangeEnd) {
		assert(rangeStart != rangeEnd);
		return (rangeStart < rangeEnd) ?
			   ((rangeStart <= currentTime) && (currentTime < rangeEnd)) :
			   ((rangeStart <= currentTime) || (currentTime < rangeEnd));
	}
}

#endif //DATA_WORLD_WORLD_HPP
