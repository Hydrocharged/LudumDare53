// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <engine/application.hpp>
#include <data/initialize.hpp>
#include <engine/utils/rollingaverage.hpp>

#include <ecs/job.hpp>

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
		return true;
	}

	void Shutdown() override {
		data::Terminate();
	}

	bool Update(double deltaTime) override {
		static engine::utils::RollingAverage<double> deltaTimesAvg(10);
		deltaTimesAvg.Update(deltaTime);
		ImVec2 initialWindowsPos((float)engine::graphics::GetWindowWidth() / 2.0f, (float)engine::graphics::GetWindowHeight() / 8.0f);
		ImGui::SetNextWindowPos(initialWindowsPos, ImGuiCond_FirstUseEver);
		ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Framerate: %d | Frametime: %.5fms", (int)(1.0 / deltaTimesAvg.GetCurrentAverage()), deltaTimesAvg.GetCurrentAverage());
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
};

engine::Application* engine::NewApplication() {
	return new DeLiverInc();
}
