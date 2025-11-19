#include "WidgetUtils.h"

#include <algorithm>

ImVec4 AdjustColor(const ImVec4& color, float factor) {
    return ImVec4(std::clamp(color.x * factor, 0.0f, 1.0f), std::clamp(color.y * factor, 0.0f, 1.0f), std::clamp(color.z * factor, 0.0f, 1.0f),
                  color.w);
}
