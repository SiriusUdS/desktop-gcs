#include "TankGasLeftPlotDataProcessor.h"

#include "AppState.h"
#include "GSDataCenter.h"
#include "Logging.h"

TankGasLeftPlotDataProcessor::TankGasLeftPlotDataProcessor()
    : PlotDataProcessor({&GSDataCenter::LoadCell_FillingStation_PlotData.tank().getAdcPlotData()}) {
}

void TankGasLeftPlotDataProcessor::processNewData() {
    const PlotData* tankLoadCellPlotData = &GSDataCenter::LoadCell_FillingStation_PlotData.tank().getAdcPlotData();

    auto tankLoadCellPlotDataUpdateIt = plotDataUpdateMap.find(tankLoadCellPlotData);
    if (tankLoadCellPlotDataUpdateIt == plotDataUpdateMap.end()) {
        GCS_APP_LOG_ERROR("TankGasLeftPlotDataProcessor: Tank temperature plot data not found in tracked plot data map.");
        return;
    }
    const PlotDataProcessData& tankLoadCellPlotDataUpdate = tankLoadCellPlotDataUpdateIt->second;

    const double tankLoadCellADCValue = static_cast<double>(tankLoadCellPlotDataUpdate.value);
    const double prewrapADCValue = static_cast<double>(AppState::TankLoadCell::prewrapADCValue.value.load());
    const double postIPAADCValue = static_cast<double>(AppState::TankLoadCell::postIPAADCValue.value.load());

    if (postIPAADCValue == prewrapADCValue) {
        GCS_APP_LOG_ERROR(
          "TankGasLeftPlotDataProcessor: prewrapADCValue and postIPAADCValue have the same value, tank gas left value not computed to "
          "avoid division by 0.");
        return;
    }

    const float timestamp = tankLoadCellPlotDataUpdate.timestamp;
    const float value_perc = static_cast<float>(100.0f * (tankLoadCellADCValue - prewrapADCValue) / (tankLoadCellADCValue - postIPAADCValue));

    GSDataCenter::TankGasLeft_perc_PlotData.addData(timestamp, value_perc);
}
