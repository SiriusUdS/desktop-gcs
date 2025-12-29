#pragma once

#include "PlotDataProcessor.h"

class TankMassPlotDataProcessor : public PlotDataProcessor {
public:
    TankMassPlotDataProcessor();

private:
    void processNewData() override;
};
