#pragma once

#include "UIWindow.h"

#include <imgui.h>

class NOSPhaseDiagramWindow : public UIWindow {
public:
    void init() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    using PT_Point = ImVec2; // Point type for Pressure-Temperature diagram

    void renderImpl() override;

    static const PT_Point triplePoint;
    static const PT_Point criticalPoint;

    static const float minTempAntoineEquation_C;
    static const float maxTempAntoineEquation_C;

    std::vector<float> vaporizationCurveTemperatures;
    std::vector<float> vaporizationCurvePressures;
    ImVector<float> topLine;
    ImVector<float> bottomLine;
};
