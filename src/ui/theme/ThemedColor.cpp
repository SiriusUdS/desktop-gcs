#include "ThemedColor.h"

#include "UITheme.h"

ImColor ThemedColor::resolve() const {
    return UITheme::isDarkTheme() ? darkTheme : lightTheme;
}
