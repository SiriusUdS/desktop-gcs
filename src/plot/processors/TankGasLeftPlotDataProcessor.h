#pragma once

#include "PlotDataProcessor.h"

class TankGasLeftPlotDataProcessor : public PlotDataProcessor {
public:
    TankGasLeftPlotDataProcessor();

private:
    void processNewData() override;
};
