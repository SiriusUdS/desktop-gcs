#include "SensorPlotData.h"

/**
 * @brief Add ADC and value data to the sensor plot.
 */
void SensorPlotData::addData(float adc, float value, float timestamp) {
    adcPlotData.addData(timestamp, adc);
    valuePlotData.addData(timestamp, value);
}

void SensorPlotData::addListenerValue(PlotDataUpdateListener* listener) {
    valuePlotData.addListener(listener);
}

/**
 * @brief Clears both ADC and value plot data.
 */
void SensorPlotData::clear() {
    adcPlotData.clear();
    valuePlotData.clear();
}

const PlotData& SensorPlotData::getAdcPlotData() const {
    return adcPlotData;
}

const PlotData& SensorPlotData::getValuePlotData() const {
    return valuePlotData;
}
