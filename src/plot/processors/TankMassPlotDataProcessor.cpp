#include "TankMassPlotDataProcessor.h"

#include "GSDataCenter.h"
#include "Logging.h"
#include "SensorPlotData.h"
#include "TankMass.h"

TankMassPlotDataProcessor::TankMassPlotDataProcessor()
    : PlotDataProcessor({&GSDataCenter::Thermistor_Motor_PlotData.tank().getValuePlotData(),
                         &GSDataCenter::PressureSensor_Motor_PlotData.tank().getValuePlotData()}) {
}

void TankMassPlotDataProcessor::processNewData() {
    const PlotData* tankTempPlotData = &GSDataCenter::Thermistor_Motor_PlotData.tank().getValuePlotData();
    const PlotData* tankPressurePlotData = &GSDataCenter::PressureSensor_Motor_PlotData.tank().getValuePlotData();

    auto tankTempPlotDataUpdateIt = plotDataUpdateMap.find(tankTempPlotData);
    if (tankTempPlotDataUpdateIt == plotDataUpdateMap.end()) {
        GCS_APP_LOG_ERROR("TankMassPlotDataProcessor: Tank temperature plot data not found in tracked plot data map.");
        return;
    }
    const PlotDataProcessData& tankTempPlotDataUpdate = tankTempPlotDataUpdateIt->second;

    auto tankPressurePlotDataUpdateIt = plotDataUpdateMap.find(tankPressurePlotData);
    if (tankPressurePlotDataUpdateIt == plotDataUpdateMap.end()) {
        GCS_APP_LOG_ERROR("TankMassPlotDataProcessor: Tank pressure plot data not found in tracked plot data map.");
        return;
    }
    const PlotDataProcessData& tankPressurePlotDataUpdate = tankPressurePlotDataUpdateIt->second;

    const double tankTemperature_C = static_cast<double>(tankTempPlotDataUpdate.value);
    const double tankPressure_psi = static_cast<double>(tankPressurePlotDataUpdate.value);
    const float tankMass = TankMass::getNOSTankMass_lb(tankTemperature_C, tankPressure_psi);

    // Get latest timestamp for new tank mass data point
    float latestX = 0.0f;
    for (const auto& [_, update] : plotDataUpdateMap) {
        latestX = std::max(latestX, update.timestamp);
    }

    if (std::isnan(tankMass)) {
        GCS_APP_LOG_ERROR("TankMassPlotDataProcessor: Computed tank mass is \"NaN\".");
    } else if (std::isinf(tankMass)) {
        GCS_APP_LOG_ERROR("TankMassPlotDataProcessor: Computed tank mass is \"inf\".");
    } else {
        GSDataCenter::NOSTankMass_PlotData.addData(latestX, tankMass);
    }
}
