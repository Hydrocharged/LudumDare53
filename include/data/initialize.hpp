// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef DATA_INITIALIZE_HPP
#define DATA_INITIALIZE_HPP

#include <data/data.hpp>

namespace data {
	bool Initialize(engine::Application* application);
	void Terminate();
}

#endif //DATA_INITIALIZE_HPP
