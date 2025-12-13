#pragma once

#include "Param.h"

#include <ini.h>

namespace ParamsIO {
void loadParams(const mINI::INIStructure& ini);
void saveParams(mINI::INIStructure& ini);

void loadFloatParam(const mINI::INIStructure& ini, FloatParam& param);
void saveFloatParam(mINI::INIStructure& ini, const FloatParam& param);
} // namespace ParamsIO
