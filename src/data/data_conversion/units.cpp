#pragma once

#include "units.h"
#include <cmath>
#include <concepts>
#include <type_traits>
#include <iostream>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/*==========================================================================
*                             LABELS & SYMBOLS
==========================================================================*/
const char* Units::as_label(Unit unit) {
    return std::visit(overloaded {
        [](WeightUnit a) { return Units::as_label(a); },
        [](ForceUnit a) { return Units::as_label(a); },
        [](PressureUnit a) { return Units::as_label(a); },
        [](TemperatureUnit a) { return Units::as_label(a); },
        [](TimeUnit a) { return Units::as_label(a); },
        [](QuantityUnit a) { return Units::as_label(a); },
        },
        unit);
}

const char* Units::as_label(WeightUnit unit) {
    switch (unit) {
        case WeightUnit::Kilograms: return "Weight (kg)";
        case WeightUnit::Pounds: return "Weight (lb)";
        default: return "Weight (Unknown)";
    }
}

const char* Units::as_label(ForceUnit unit) {
    switch (unit) {
        case ForceUnit::Newtons: return "Force (N)";
        case ForceUnit::PoundsForce: return "Force (lbf)";
        case ForceUnit::KilogramsForce: return "Force (kgf)";
        default: return "Force (Unknown)";
    }
}
const char* Units::as_label(PressureUnit unit) {
    switch (unit) {
        case PressureUnit::Psi: return "Pressure (psi)";
        case PressureUnit::Kilopascals: return "Pressure (kPa)";
        default: return "Pressure (Unknown)";
    }
}
const char* Units::as_label(TemperatureUnit unit) {
    switch (unit) {
        case TemperatureUnit::Celcius: return "Temperature (°C)";
        case TemperatureUnit::Fahrenheit: return "Temperature (°F)";
        case TemperatureUnit::Kelvin: return "Temperature (K)";
        default: return "Temperature (Unknown)";
    }
}
const char* Units::as_label(TimeUnit unit) {
    switch (unit) {
        case TimeUnit::Seconds: return "Time (s)";
        case TimeUnit::Milliseconds: return "Time (ms)";
        default: return "Time (Unknown)";
    }
}
const char* Units::as_label(QuantityUnit unit) {
    switch (unit) {
        case QuantityUnit::Scalar: return "Quantity";
        case QuantityUnit::Percentage: return "Quantity (%)";
        default: return "Quantity (Unknown)";
    }
}

const char* Units::as_symbol(Unit unit) {
    return std::visit(overloaded {
        [](WeightUnit a) { return Units::as_symbol(a); },
        [](ForceUnit a) { return Units::as_symbol(a); },
        [](PressureUnit a) { return Units::as_symbol(a); },
        [](TemperatureUnit a) { return Units::as_symbol(a); },
        [](TimeUnit a) { return Units::as_symbol(a); },
        [](QuantityUnit a) { return Units::as_symbol(a); },
        },
        unit);
}
const char* Units::as_symbol(WeightUnit unit) {
    switch (unit) {
        case WeightUnit::Kilograms: return "kg";
        case WeightUnit::Pounds: return "lb";
        default: return "?";
    }
}
const char* Units::as_symbol(ForceUnit unit) {
    switch (unit) {
        case ForceUnit::Newtons: return "N";
        case ForceUnit::PoundsForce: return "lbf";
        case ForceUnit::KilogramsForce: return "kgf";
        default: return "?";
    }
}
const char* Units::as_symbol(PressureUnit unit) {
    switch (unit) {
        case PressureUnit::Psi: return "psi";
        case PressureUnit::Kilopascals: return "kPa";
        default: return "?";
    }
}
const char* Units::as_symbol(TemperatureUnit unit) {
    switch (unit) {
        case TemperatureUnit::Celcius: return "°C";
        case TemperatureUnit::Fahrenheit: return "°F";
        case TemperatureUnit::Kelvin: return "K";
        default: return "?";
    }
}
const char* Units::as_symbol(TimeUnit unit) {
    switch (unit) {
        case TimeUnit::Seconds: return "s";
        case TimeUnit::Milliseconds: return "ms";
        default: return "?";
    }
}
const char* Units::as_symbol(QuantityUnit unit) {
    switch (unit) {
        case QuantityUnit::Scalar: return "";
        case QuantityUnit::Percentage: return "%";
        default: return "?";
    }
}

/*==========================================================================
*                            CONVERSION FACTORS
==========================================================================*/
float Units::getConversionFactor(WeightUnit from, WeightUnit to) {
    if (from == to)
        return 1.0f; // No conversion needed

    float to_kilogram;
    switch (from) {
    case WeightUnit::Kilograms:
        to_kilogram = 1.0f;
        break;
    case WeightUnit::Pounds:
        to_kilogram = POUNDS_TO_KILOGRAMS;
        break;
    default:
        return NAN; // Invalid unit
    };

    switch (to) {
    case WeightUnit::Kilograms:
        return to_kilogram;
    case WeightUnit::Pounds:
        return to_kilogram * KILOGRAMS_TO_POUNDS;
    default:
        return NAN; // Invalid unit
    };
}

float Units::getConversionFactor(ForceUnit from, ForceUnit to) {
    if (from == to)
        return 1.0f; // No conversion needed

    float to_newtons;
    switch (from) {
    case ForceUnit::Newtons:
        to_newtons = 1.0f;
        break;
    case ForceUnit::PoundsForce:
        to_newtons = POUNDS_FORCE_TO_NEWTONS;
        break;
    case ForceUnit::KilogramsForce:
        to_newtons = KILOGRAMS_FORCE_TO_NEWTONS;
        break;
    default:
        return NAN; // Invalid unit
    };

    switch (to) {
    case ForceUnit::Newtons:
        return to_newtons;
    case ForceUnit::PoundsForce:
        return to_newtons * NEWTONS_TO_POUNDS_FORCE;
    case ForceUnit::KilogramsForce:
        return to_newtons * NEWTONS_TO_KILOGRAMS_FORCE;
    default:
        return NAN; // Invalid unit
    };
}

float Units::getConversionFactor(PressureUnit from, PressureUnit to) {
    if (from == to)
        return 1.0f; // No conversion needed

    float to_kilopascals;
    switch (from) {
    case PressureUnit::Kilopascals:
        to_kilopascals = 1.0f;
        break;
    case PressureUnit::Psi:
        to_kilopascals = PSI_TO_KILOPASCALS;
        break;
    default:
        return NAN; // Invalid unit
    };

    switch (to) {
    case PressureUnit::Kilopascals:
        return to_kilopascals;
    case PressureUnit::Psi:
        return to_kilopascals * KILOPASCALS_TO_PSI;
    default:
        return NAN; // Invalid unit
    };
}
float Units::getConversionFactor(TimeUnit from, TimeUnit to) {
    if (from == to)
        return 1.0f; // No conversion needed

    float seconds;
    switch (from) {
    case TimeUnit::Seconds:
        seconds = 1.0f;
        break;
    case TimeUnit::Milliseconds:
        seconds = 0.001f;
        break;
    default:
        return NAN; // Invalid unit
    };

    switch (to) {
    case TimeUnit::Seconds:
        return seconds;
    case TimeUnit::Milliseconds:
        return seconds * 1000.0f;
    default:
        return NAN; // Invalid unit
    };
}
float Units::getConversionFactor(QuantityUnit from, QuantityUnit to) {
    if (from == to)
        return 1.0f; // No conversion needed

    float scalar;
    switch (from) {
    case QuantityUnit::Scalar:
        scalar = 1.0f;
        break;
    case QuantityUnit::Percentage:
        scalar = 100.0f;
        break;
    default:
        return NAN; // Invalid unit
    };

    switch (to) {
    case QuantityUnit::Scalar:
        return scalar;
    case QuantityUnit::Percentage:
        return scalar * 0.01f;
    default:
        return NAN; // Invalid unit
    };
}

/*==========================================================================
*                                 CONVERSIONS
==========================================================================*/

float Units::convert(float value, Unit from, Unit to) {
    return std::visit(overloaded {
        [value](WeightUnit a, WeightUnit b) { return Units::convert(value, a, b); },
        [value](ForceUnit a, ForceUnit b) { return Units::convert(value, a, b); },
        [value](PressureUnit a, PressureUnit b) { return Units::convert(value, a, b); },
        [value](TemperatureUnit a, TemperatureUnit b) { return Units::convert(value, a, b); },
        [value](TimeUnit a, TimeUnit b) { return Units::convert(value, a, b); },
        [value](QuantityUnit a, QuantityUnit b) { return Units::convert(value, a, b); },
        [](auto, auto) { std::cerr << "Runtime Error: Mismatched unit types inside the containers!\n"; return NAN; }
        },
        from, to);
}
float Units::convert(float value, WeightUnit from, WeightUnit to) {
    return value * getConversionFactor(from, to);
}
float Units::convert(float value, ForceUnit from, ForceUnit to) {
    return value * getConversionFactor(from, to);
}
float Units::convert(float value, PressureUnit from, PressureUnit to) {
    return value * getConversionFactor(from, to);
}
float Units::convert(float value, TemperatureUnit from, TemperatureUnit to) {
    if (from == to) return value; // No conversion needed

    float celcius;
    switch (from) {
        case TemperatureUnit::Celcius: celcius = value;
            break;
        case TemperatureUnit::Fahrenheit: celcius = (value - 32.0f) * 5.0f / 9.0f;
            break;
        case TemperatureUnit::Kelvin: celcius = value - 273.15f;
            break;
        default: return NAN; // Invalid unit
    }

    switch (to) {
        case TemperatureUnit::Celcius: return celcius;
        case TemperatureUnit::Fahrenheit: return celcius * 9.0f / 5.0f + 32.0f;
        case TemperatureUnit::Kelvin: return celcius + 273.15f;
        default: return NAN; // Invalid unit
    }
}
float Units::convert(float value, TimeUnit from, TimeUnit to) {
    return value * getConversionFactor(from, to);
}
float Units::convert(float value, QuantityUnit from, QuantityUnit to) {
    return value * getConversionFactor(from, to);
}