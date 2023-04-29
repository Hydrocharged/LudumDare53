// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef ECS_JOB_HPP
#define ECS_JOB_HPP

#include <engine/application.hpp>

namespace ecs {
	class Job {
	public:
		Job();
		~Job();

		virtual bool Run(double deltaTime) = 0;
		[[nodiscard]] virtual std::uint8_t BatchID() const = 0;

		static void SetApplication(engine::Application* application);

	protected:
		static engine::Application* application();
	};
}

#endif //ECS_JOB_HPP
