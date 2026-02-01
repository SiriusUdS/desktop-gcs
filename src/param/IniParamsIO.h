#pragma once

#include "IniParam.h"

#include <ini.h>

namespace IniParamsIO {
void loadParams(const mINI::INIStructure& ini);
void saveParams(mINI::INIStructure& ini);

void loadFloatParam(const mINI::INIStructure& ini, FloatIniParam& param);
void saveFloatParam(mINI::INIStructure& ini, const FloatIniParam& param);

void loadBoolParam(const mINI::INIStructure& ini, BoolIniParam& param);
void saveBoolParam(mINI::INIStructure& ini, const BoolIniParam& param);
} // namespace IniParamsIO
