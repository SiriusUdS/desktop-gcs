#include "VaporPressure.h"

#include <cmath>

double VaporPressure::vaporPressureNOS_psi(float temperature_C) {
    // Reference: Nitrous oxide: Saturation properties and the phase diagram
    // DOI: 10.1016/j.jct.2009.06.017;
    // 3.1. Vapourization, Equation (1)

    constexpr double tc = 309.548; // K
    constexpr double pc = 7238;    // kPa
    constexpr double a1 = -6.8657;
    constexpr double a2 = 1.9373;
    constexpr double a3 = -2.6440;
    constexpr double a4 = 0.0387;
    const double t = temperature_C + 273.15; // K
    const double tau = 1 - (t / tc);
    const double p = pc * std::exp((a1 * tau + a2 * std::pow(tau, 1.5) + a3 * std::pow(tau, 2.5) + a4 * std::pow(tau, 5)) / (t / tc)); // kPa

    return p * 0.145037738; // psi
}
