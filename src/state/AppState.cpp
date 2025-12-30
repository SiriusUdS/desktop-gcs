#include "AppState.h"

namespace AppState {
namespace TankLoadCell {
State prewrapADCValue{0.0f, "prewrap_adc_value"};
State postwrapADCValue{0.0f, "postwrap_adc_value"};
State postIPAADCValue{0.0f, "postipa_adc_value"};
State postNOSADCValue{0.0f, "postnos_adc_value"};
} // namespace TankLoadCell
} // namespace AppState
