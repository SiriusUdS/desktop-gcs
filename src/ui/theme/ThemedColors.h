#pragma once

#include "ThemedColor.h"

namespace ThemedColors {
namespace Button {
const ThemedColor red{.lightTheme{255, 50, 50}, .darkTheme{200, 0, 0}};
const ThemedColor green{.lightTheme{0, 200, 50}, .darkTheme{0, 150, 30}};
const ThemedColor blue{.lightTheme{40, 165, 255}, .darkTheme{0, 120, 200}};
const ThemedColor yellow{.lightTheme{215, 215, 0}, .darkTheme{150, 150, 0}};
} // namespace Button

namespace PlotLine {
const ThemedColor red{.lightTheme{200, 30, 30}, .darkTheme{255, 80, 80}};
const ThemedColor orange{.lightTheme{200, 110, 10}, .darkTheme{255, 150, 40}};
const ThemedColor yellow{.lightTheme{190, 170, 20}, .darkTheme{255, 230, 60}};
const ThemedColor green{.lightTheme{30, 150, 40}, .darkTheme{80, 220, 80}};
const ThemedColor blue{.lightTheme{40, 80, 185}, .darkTheme{100, 150, 255}};
const ThemedColor cyan{.lightTheme{0, 145, 145}, .darkTheme{60, 220, 220}};
const ThemedColor purple{.lightTheme{110, 55, 175}, .darkTheme{170, 110, 255}};
const ThemedColor gray{.lightTheme{120, 120, 120}, .darkTheme{200, 200, 200}};
} // namespace PlotLine

namespace StateMachine {
const ThemedColor activeState{.lightTheme{170, 255, 170}, .darkTheme{0, 110, 0}};
const ThemedColor inactiveState{.lightTheme{255, 170, 170}, .darkTheme{140, 0, 0}};
const ThemedColor arrow{.lightTheme{0, 0, 0}, .darkTheme{255, 255, 255}};
} // namespace StateMachine
} // namespace ThemedColors
