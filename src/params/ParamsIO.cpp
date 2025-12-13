#include "ParamsIO.h"

#include "IniConfig.h"
#include "Params.h"

void ParamsIO::loadParams(const mINI::INIStructure& ini) {
    if (ini.has(IniConfig::GCS_SECTION)) {
        loadFloatParam(ini, Params::NOSTankVolume_m3);
        loadFloatParam(ini, Params::TankLoadCell::prewrapADCValue);
        loadFloatParam(ini, Params::TankLoadCell::postwrapADCValue);
        loadFloatParam(ini, Params::TankLoadCell::postIPAADCValue);
    }
}

void ParamsIO::saveParams(mINI::INIStructure& ini) {
    saveFloatParam(ini, Params::NOSTankVolume_m3);
    saveFloatParam(ini, Params::TankLoadCell::prewrapADCValue);
    saveFloatParam(ini, Params::TankLoadCell::postwrapADCValue);
    saveFloatParam(ini, Params::TankLoadCell::postIPAADCValue);
}

void ParamsIO::loadFloatParam(const mINI::INIStructure& ini, FloatParam& param) {
    if (ini.get(IniConfig::GCS_SECTION).has(param.iniKey)) {
        param.currentValue = std::stof(ini.get(IniConfig::GCS_SECTION).get(param.iniKey));
    }
}

void ParamsIO::saveFloatParam(mINI::INIStructure& ini, const FloatParam& param) {
    ini[IniConfig::GCS_SECTION].set(param.iniKey, std::to_string(param.currentValue));
}
