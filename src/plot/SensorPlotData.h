#pragma once

#include "PlotData.h"
#include "units.h"

class PlotDataUpdateListener;

enum class PlotMode {
    Value,
    ADC
};

class SensorPlotData {
public:
    SensorPlotData(Units::TimeUnit time_unit, Units::Unit unit, Units::Unit adc_unit = Units::QuantityUnit::Scalar);

    void addData(float adc, float value, float timestamp);
    void addListenerAdc(PlotDataUpdateListener* listener);
    void addListenerValue(PlotDataUpdateListener* listener);

    void clear();
    size_t getSize() const;
    const PlotData& getAdcPlotData() const;
    const PlotData& getValuePlotData() const;
    const PlotData& getPlotData(PlotMode mode) const;
    Units::Unit getUnit(PlotMode mode) const;

private:
    PlotData adcPlotData;
    PlotData valuePlotData;
};