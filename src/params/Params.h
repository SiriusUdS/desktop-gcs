#pragma once

#include <ini.h>

namespace Params {
void loadParams(const mINI::INIStructure& ini);
void saveParams(mINI::INIStructure& ini);
} // namespace Params
