#pragma once

#include "CommandTypes.h"
#include "UIWindow.h"

class ControlsWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    /**
     * @struct ValveSlider
     * @brief Holds the state of a valve slider.
     */
    struct PercentageInput {
        int openedValue_perc{};        ///< Value in percentage of the input.
        int lastSetOpenedValue_perc{}; ///< Value in percentage of the input that was last sent.
        int lastOpenedValue_perc{};    ///< Last value sent to the command control.
    };

    PercentageInput nosValveSlider;
    PercentageInput ipaValveSlider;
    PercentageInput fillValveSlider;
    PercentageInput dumpValveSlider;
    PercentageInput nosHeatPadSlider;
    PercentageInput ipaHeatPadSlider;
    PercentageInput fillHeatPadSlider;
    PercentageInput dumpHeatPadSlider;

    void renderImpl() override;
    void renderPercentageInput(const char* name,
                               PercentageInput& slider,
                               CommandType commandType,
                               const char* tooltipDisabled = "",
                               bool inputEnabled = true,
                               bool onlyFullyClosedOrOpen = false) const;
};
