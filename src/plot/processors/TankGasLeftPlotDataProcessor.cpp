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

    if (!AppState::TankLoadCell::prewrapADCValue.saved || !AppState::TankLoadCell::postIPAADCValue.saved) {
        // Prewrap and Post IPA tank load cell ADC values need to be confirmed/saved before the "tank gas left" value can be determined.
        return;
    }

    const float current = tankLoadCellPlotDataUpdate.value;
    const float prewrap = AppState::TankLoadCell::prewrapADCValue.value;
    const float postIPA = AppState::TankLoadCell::postIPAADCValue.value;

    if (postIPA == prewrap) {
        // Returning early to avoid division by 0.
        return;
    }

    const float timestamp = tankLoadCellPlotDataUpdate.timestamp;
    const float value_perc = 100.0f * (current - prewrap) / (postIPA - prewrap);

    GSDataCenter::TankGasLeft_perc_PlotData.addData(timestamp, value_perc);
}
