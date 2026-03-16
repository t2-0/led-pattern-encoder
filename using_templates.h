#pragma once
#include <array>
#include "Led.h"

using std::array;

using LedStateMatrix8 = array<array<LedState, 8>, 8>;
template<size_t N>
using DisplayStates = array<LedStateMatrix8, N>;

using LedMatrix8 = array<array<Led, 8>, 8>;
template<size_t N>
using Display = array<LedMatrix8, N>;