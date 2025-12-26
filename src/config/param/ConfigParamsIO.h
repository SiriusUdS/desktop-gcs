#pragma once

#include "ConfigParam.h"

#include <ini.h>

namespace ConfigParamsIO {
void loadParams(const mINI::INIStructure& ini);
void saveParams(mINI::INIStructure& ini);

void loadFloatParam(const mINI::INIStructure& ini, FloatConfigParam& param);
void saveFloatParam(mINI::INIStructure& ini, const FloatConfigParam& param);
} // namespace ConfigParamsIO
