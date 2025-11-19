#include "TankMass.h"

#include "RocketParams.h"

#include <CoolPropLib.h>
#include <cmath>
#include <limits>

float TankMass::getNOSTankMass_lb(double tankTemperature_C, double tankPressure_psi) {
    const double tankTemperature_K = tankTemperature_C + 273.5;
    const double tankPressure_Pa = tankPressure_psi * 6894.76;
    const double rho = PropsSI("D", "T", tankTemperature_K, "P", tankPressure_Pa, "NitrousOxide");

    if (std::isnan(rho) || rho < 0) {
        return std::numeric_limits<float>::quiet_NaN();
    } else if (std::isinf(rho)) {
        return std::numeric_limits<float>::infinity();
    }

    const double tankMass_kg = rho * RocketParams::NOSTankVolume_m3.currentValue;
    return (float) (tankMass_kg * 2.20462262185);
}
