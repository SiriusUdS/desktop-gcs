#pragma once

#include <imgui.h>

struct ThemedColor {
    ImColor lightTheme;
    ImColor darkTheme;

    ImColor resolve() const;
};
