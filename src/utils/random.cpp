// Copyright © 2023 Daylon Wilkins & James Cor
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <ldutils/random.hpp>
#include <random>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(0.0, std::nextafter(1.0, 2.0));
std::uniform_real_distribution<float> distf(0.0f, std::nextafter(1.0f, 2.0f));

double ldutils::Random() {
	return dist(mt);
}

float ldutils::Randomf() {
	return distf(mt);
}
