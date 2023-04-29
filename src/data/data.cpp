// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <data/initialize.hpp>
#include <data/player/initialize.hpp>
#include <data/world/initialize.hpp>

bool data::Initialize(engine::Application* application) {
	if (!data::player::Initialize(application)) {
		return false;
	}
	if (!data::world::Initialize(application)) {
		return false;
	}
	return true;
}

void data::Terminate() {
	data::world::Terminate();
	data::player::Terminate();
}
