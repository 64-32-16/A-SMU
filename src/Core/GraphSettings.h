#pragma once
#include <stdint.h>

// Shared configuration, independent of UI controls and transient touch state.
// Explicit enum values form the basis for a future versioned setup file.
enum class GraphStyle : uint8_t { Lines=0, Points=1, Both=2 };
struct GraphSettings {
    GraphStyle style=GraphStyle::Lines;
};
