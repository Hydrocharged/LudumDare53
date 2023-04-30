// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <engine/application.hpp>
#include <data/initialize.hpp>
#include <engine/utils/rollingaverage.hpp>

#include <ecs/ecs.hpp>
#include <ecs/jobs/civiliansim.hpp>
#include <ecs/jobs/driving.hpp>
#include <ecs/jobs/employeesim.hpp>
#include <ecs/jobs/world.hpp>

#include <generation/civilian.hpp>
#include <generation/employee.hpp>

class DeLiverInc : public engine::Application {
public:
	DeLiverInc() = default;
	~DeLiverInc() override = default;

	bool Initialize() override {
		engine::graphics::SetVSync(true);
		engine::graphics::SetFramerateLimit(500);
		ecs::Job::SetApplication(this);
		if (!data::Initialize(this)) {
			return false;
		}

		// Generate World
		//TODO: generate the buildings
		generation::CreateCivilian(70);
		generation::CreateEmployee(10);

		// Create Systems (ordered by execution)
		world = std::make_unique<ecs::jobs::World>();
		driving = std::make_unique<ecs::jobs::Driving>();
		civilianSim = std::make_unique<ecs::jobs::CivilianSim>();
		employeeSim = std::make_unique<ecs::jobs::EmployeeSim>();
		return true;
	}

	void Shutdown() override {
		data::Terminate();
	}

	bool Update(double deltaTime) override {
		// Display Framerate
		static engine::utils::RollingAverage<double> deltaTimesAvg(10);
		deltaTimesAvg.Update(deltaTime);
		ImVec2 initialWindowsPos((float)engine::graphics::GetWindowWidth() / 2.0f, (float)engine::graphics::GetWindowHeight() / 8.0f);
		ImGui::SetNextWindowPos(initialWindowsPos, ImGuiCond_FirstUseEver);
		ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Framerate: %d | Frametime: %.5fms", (int)(1.0 / deltaTimesAvg.GetCurrentAverage()), deltaTimesAvg.GetCurrentAverage());
		ImGui::End();

		// Run systems
		world->SetDeltaTime(deltaTime);
		ecs::Entity::World().progress(1.0f);

		//TODO: REMOVE DEBUG STUFF
		ImGui::Begin("Internal Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		int currentDay = int(glm::floor(data::world::DayTime / HOURS_24));
		int currentHours = (int(data::world::DayTime) % int(HOURS_24F)) / 3600;
		int currentMinutes = (int(data::world::DayTime) % 3600) / 60;
		int currentSeconds = int(data::world::DayTime) % 60;
		ImGui::LabelText("Current Day", "Day %d: %02d:%02d:%02d", currentDay, currentHours, currentMinutes, currentSeconds);
		ImGui::LabelText("Money", "$%.2f", data::player::Money);
		ImGui::LabelText("Gas Prices", "$%.2f", data::world::GasPrice);
		float gs = data::world::GameSpeed;
		ImGui::SliderFloat("Game Speed", &gs, 1.0f, 36000.0f);
		data::world::GameSpeed = double(gs);
		ImGui::End();
		return true;
	}

	bool FixedUpdate(double deltaTime) override {
		return true;
	}

	bool Draw(double deltaTime) override {
		return true;
	}

	engine::ApplicationOptions StartOptions() override {
		engine::ApplicationOptions options{
			.Title = "De-Liver Inc.",
			.Width = 1280,
			.Height = 720,
		};
		return options;
	}

private:
	std::unique_ptr<ecs::jobs::World> world;
	std::unique_ptr<ecs::jobs::Driving> driving;
	std::unique_ptr<ecs::jobs::CivilianSim> civilianSim;
	std::unique_ptr<ecs::jobs::EmployeeSim> employeeSim;
};

engine::Application* engine::NewApplication() {
	return new DeLiverInc();
}
