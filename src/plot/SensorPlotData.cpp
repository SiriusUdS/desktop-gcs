#include "SensorPlotData.h"

SensorPlotData::SensorPlotData(Units::TimeUnit time_unit, Units::Unit unit, Units::Unit adc_unit): valuePlotData(time_unit, unit), adcPlotData(time_unit, adc_unit) {}

void SensorPlotData::addData(float adc, float value, float timestamp) {
    adcPlotData.addData(timestamp, adc);
    valuePlotData.addData(timestamp, value);
}

void SensorPlotData::addListenerAdc(PlotDataUpdateListener* listener) {
    adcPlotData.addListener(listener);
}

void SensorPlotData::addListenerValue(PlotDataUpdateListener* listener) {
    valuePlotData.addListener(listener);
}

void SensorPlotData::clear() {
    adcPlotData.clear();
    valuePlotData.clear();
}

size_t SensorPlotData::getSize() const {
    return valuePlotData.getSize();
}

const PlotData& SensorPlotData::getAdcPlotData() const {
    return adcPlotData;
}

const PlotData& SensorPlotData::getValuePlotData() const {
    return valuePlotData;
}

const PlotData& SensorPlotData::getPlotData(PlotMode mode) const {
    return mode == PlotMode::Value ? valuePlotData : adcPlotData;
}

Units::Unit SensorPlotData::getUnit(PlotMode mode) const {
    return mode == PlotMode::Value ? valuePlotData.getValues().getUnit() : adcPlotData.getValues().getUnit();
}