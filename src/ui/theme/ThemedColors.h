#ifndef THEMEDCOLORS_H
#define THEMEDCOLORS_H

#include "ThemedColor.h"

namespace ThemedColors {
const ThemedColor RED_BUTTON{.lightTheme = {255, 50, 50}, .darkTheme = {200, 0, 0}};
const ThemedColor GREEN_BUTTON{.lightTheme = {0, 200, 50}, .darkTheme = {0, 150, 30}};
const ThemedColor BLUE_BUTTON{.lightTheme = {40, 165, 255}, .darkTheme = {0, 120, 200}};
const ThemedColor YELLOW_BUTTON{.lightTheme = {215, 215, 0}, .darkTheme = {150, 150, 0}};

const ThemedColor RED_PLOT_LINE{.lightTheme = {200, 30, 30}, .darkTheme = {255, 80, 80}};
const ThemedColor ORANGE_PLOT_LINE{.lightTheme = {200, 110, 10}, .darkTheme = {255, 150, 40}};
const ThemedColor YELLOW_PLOT_LINE{.lightTheme = {190, 170, 20}, .darkTheme = {255, 230, 60}};
const ThemedColor GREEN_PLOT_LINE{.lightTheme = {30, 150, 40}, .darkTheme = {80, 220, 80}};
const ThemedColor BLUE_PLOT_LINE{.lightTheme = {40, 80, 185}, .darkTheme = {100, 150, 255}};
const ThemedColor CYAN_PLOT_LINE{.lightTheme = {0, 145, 145}, .darkTheme = {60, 220, 220}};
const ThemedColor PURPLE_PLOT_LINE{.lightTheme = {110, 55, 175}, .darkTheme = {170, 110, 255}};
const ThemedColor GRAY_PLOT_LINE{.lightTheme = {120, 120, 120}, .darkTheme = {200, 200, 200}};
} // namespace ThemedColors

#endif // THEMEDCOLORS_H
