#include "ConfigParamsIO.h"

#include "ConfigParams.h"
#include "IniConfig.h"

void ConfigParamsIO::loadParams(const mINI::INIStructure& ini) {
    if (ini.has(IniConfig::GCS_SECTION)) {
        loadBoolParam(ini, ConfigParams::compressPlots);
        loadFloatParam(ini, ConfigParams::NOSTankVolume_m3);
    }
}

void ConfigParamsIO::saveParams(mINI::INIStructure& ini) {
    saveBoolParam(ini, ConfigParams::compressPlots);
    saveFloatParam(ini, ConfigParams::NOSTankVolume_m3);
}

void ConfigParamsIO::loadFloatParam(const mINI::INIStructure& ini, FloatConfigParam& param) {
    if (ini.get(IniConfig::GCS_SECTION).has(param.iniKey)) {
        param.currentValue = std::stof(ini.get(IniConfig::GCS_SECTION).get(param.iniKey));
    }
}

void ConfigParamsIO::saveFloatParam(mINI::INIStructure& ini, const FloatConfigParam& param) {
    ini[IniConfig::GCS_SECTION].set(param.iniKey, std::to_string(param.currentValue));
}

void ConfigParamsIO::loadBoolParam(const mINI::INIStructure& ini, BoolConfigParam& param) {
    if (ini.get(IniConfig::GCS_SECTION).has(param.iniKey)) {
        param.currentValue = std::stoi(ini.get(IniConfig::GCS_SECTION).get(param.iniKey));
    }
}

void ConfigParamsIO::saveBoolParam(mINI::INIStructure& ini, const BoolConfigParam& param) {
    ini[IniConfig::GCS_SECTION].set(param.iniKey, std::to_string(param.currentValue));
}
