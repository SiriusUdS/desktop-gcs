#ifndef THEMEDCOLOR_H
#define THEMEDCOLOR_H

#include <imgui.h>

struct ThemedColor {
    ImColor lightTheme;
    ImColor darkTheme;

    ImColor resolve() const;
};

#endif // THEMEDCOLOR_H
