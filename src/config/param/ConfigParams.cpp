#include "ConfigParams.h"

namespace ConfigParams {
FloatConfigParam NOSTankVolume_m3(0.060516f, "params_rocket_tank_volume_m3");

namespace TankLoadCell {
FloatConfigParam prewrapADCValue{0.0f, "params_tank_load_cell_prewrap_adc_value"};
FloatConfigParam postwrapADCValue{0.0f, "params_tank_load_cell_postwrap_adc_value"};
FloatConfigParam postIPAADCValue{0.0f, "params_tank_load_cell_postipa_adc_value"};
} // namespace TankLoadCell
} // namespace ConfigParams
