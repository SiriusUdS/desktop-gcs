#include "IniParamsIO.h"

#include "IniConfig.h"
#include "IniParams.h"

void IniParamsIO::loadParams(const mINI::INIStructure& ini) {
    if (ini.has(IniConfig::GCS_SECTION)) {
        loadBoolParam(ini, IniParams::compressPlots);
        loadFloatParam(ini, IniParams::NOSTankVolume_m3);
    }
}

void IniParamsIO::saveParams(mINI::INIStructure& ini) {
    saveBoolParam(ini, IniParams::compressPlots);
    saveFloatParam(ini, IniParams::NOSTankVolume_m3);
}

void IniParamsIO::loadFloatParam(const mINI::INIStructure& ini, FloatIniParam& param) {
    if (ini.get(IniConfig::GCS_SECTION).has(param.iniKey)) {
        param.currentValue = std::stof(ini.get(IniConfig::GCS_SECTION).get(param.iniKey));
    }
}

void IniParamsIO::saveFloatParam(mINI::INIStructure& ini, const FloatIniParam& param) {
    ini[IniConfig::GCS_SECTION].set(param.iniKey, std::to_string(param.currentValue));
}

void IniParamsIO::loadBoolParam(const mINI::INIStructure& ini, BoolIniParam& param) {
    if (ini.get(IniConfig::GCS_SECTION).has(param.iniKey)) {
        param.currentValue = std::stoi(ini.get(IniConfig::GCS_SECTION).get(param.iniKey));
    }
}

void IniParamsIO::saveBoolParam(mINI::INIStructure& ini, const BoolIniParam& param) {
    ini[IniConfig::GCS_SECTION].set(param.iniKey, std::to_string(param.currentValue));
}
