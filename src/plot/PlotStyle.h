#ifndef PLOTSTYLE_H
#define PLOTSTYLE_H

#include <imgui.h>

struct ThemedColor;

/**
 * @struct PlotStyle
 * @brief Style parameters for an ImPlot plot line
 */
struct PlotStyle {
    const char* name;         ///< Name of the plot line, shown in the plot's legend
    const ThemedColor& color; ///< Color of the plot line
    float weight;             ///< Thickness of the plot line
};

#endif // PLOTSTYLE_H
