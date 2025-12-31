#include "ConfigParams.h"

namespace ConfigParams {
BoolConfigParam compressPlots(false, "params_compress_plots");
FloatConfigParam NOSTankVolume_m3(0.060516f, "params_rocket_tank_volume_m3");
} // namespace ConfigParams
