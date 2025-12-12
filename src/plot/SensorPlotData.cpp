#include "SensorPlotData.h"

/**
 * @brief Constructor for SensorPlotData.
 */
SensorPlotData::SensorPlotData(const char* name, const ThemedColor& color) : adcPlotData(name, color), valuePlotData(name, color) {
}

/**
 * @brief Add ADC and value data to the sensor plot.
 */
void SensorPlotData::addData(float adc, float value, float timestamp) {
    adcPlotData.addData(timestamp, adc);
    valuePlotData.addData(timestamp, value);
}

/**
 * @brief Clears both ADC and value plot data.
 */
void SensorPlotData::clear() {
    adcPlotData.clear();
    valuePlotData.clear();
}

void SensorPlotData::addListenerValue(PlotDataUpdateListener* listener) {
    valuePlotData.addListener(listener);
}

/**
 * @brief Plot the ADC data for the sensor.
 */
void SensorPlotData::plotAdc(bool showCompressedData) const {
    adcPlotData.plot(showCompressedData);
}

/**
 * @brief Plot the value data for the sensor.
 */
void SensorPlotData::plotValue(bool showCompressedData) const {
    valuePlotData.plot(showCompressedData);
}

/**
 * @brief Compute the average adc value of the data in the last x milliseconds.
 * @param durationMs The duration in milliseconds in which we measure the recent average adc value.
 * @returns The recent average adc value.
 */
float SensorPlotData::averageRecentAdc(size_t durationMs) const {
    return adcPlotData.recentAverageValue(durationMs);
}

/**
 * @brief Compute the average value of the data in the last x milliseconds.
 * @param durationMs The duration in milliseconds in which we measure the recent average value.
 * @returns The recent average value.
 */
float SensorPlotData::averageRecentValue(size_t durationMs) const {
    return valuePlotData.recentAverageValue(durationMs);
}

size_t SensorPlotData::getSize() const {
    return adcPlotData.getSize();
}

/**
 * @brief Get the name of the sensor plot data.
 * @returns The name of the sensor plot data.
 */
const char* SensorPlotData::getName() const {
    return valuePlotData.getName();
}

const PlotData& SensorPlotData::getAdcPlotData() const {
    return adcPlotData;
}

const PlotData& SensorPlotData::getValuePlotData() const {
    return valuePlotData;
}
