#pragma once

#include <imgui.h>

struct ThemedColor;

/**
 * @struct PlotStyle
 * @brief Style parameters for an ImPlot plot line
 */
struct PlotStyle {
    PlotStyle(const char* name, const ThemedColor& color, float weight = 4) : name(name), color(color), weight(weight) {
    }

    const char* name;         ///< Name of the plot line, shown in the plot's legend
    const ThemedColor& color; ///< Color of the plot line
    float weight;             ///< Thickness of the plot line
};
