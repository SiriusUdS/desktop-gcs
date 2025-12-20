#pragma once

#include "PlotData.h"

class PlotDataUpdateListener;
struct ThemedColor;

/**
 * @class SensorPlotData
 * @brief Plot data for sensor that includes ADC and value data.
 */
class SensorPlotData {
public:
    void addData(float adc, float value, float timestamp);
    void addListenerAdc(PlotDataUpdateListener* listener);
    void addListenerValue(PlotDataUpdateListener* listener);
    void clear();
    size_t getSize() const;
    const PlotData& getAdcPlotData() const;
    const PlotData& getValuePlotData() const;

private:
    PlotData adcPlotData;
    PlotData valuePlotData;
};
