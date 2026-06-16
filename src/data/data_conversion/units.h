#pragma once
#include <concepts>
#include <variant>

namespace Units {
	const float KILOGRAMS_TO_POUNDS = 2.20462f;
	const float POUNDS_TO_KILOGRAMS = 1.0f / KILOGRAMS_TO_POUNDS;

	const float NEWTONS_TO_POUNDS_FORCE = 0.224809f;
    const float POUNDS_FORCE_TO_NEWTONS = 1.0f / NEWTONS_TO_POUNDS_FORCE;

    const float KILOGRAMS_FORCE_TO_NEWTONS = 9.80665f;
    const float NEWTONS_TO_KILOGRAMS_FORCE = 1.0f / KILOGRAMS_FORCE_TO_NEWTONS;

	const float PSI_TO_KILOPASCALS = 6.89476f;
    const float KILOPASCALS_TO_PSI = 1.0f / PSI_TO_KILOPASCALS;

	enum class WeightUnit { Kilograms, Pounds };
    enum class ForceUnit { Newtons, PoundsForce, KilogramsForce };
    enum class PressureUnit { Psi, Kilopascals };
    enum class TemperatureUnit { Celcius, Fahrenheit, Kelvin };
    enum class TimeUnit {Seconds, Milliseconds};
    enum class QuantityUnit {Scalar, Percentage};

    using Unit = std::variant<WeightUnit, ForceUnit, PressureUnit, TemperatureUnit, TimeUnit, QuantityUnit>;

    const WeightUnit DEFAULT_WEIGHT_UNIT = WeightUnit::Kilograms;
    const ForceUnit DEFAULT_FORCE_UNIT = ForceUnit::Newtons;
    const PressureUnit DEFAULT_PRESSURE_UNIT = PressureUnit::Psi;
    const TemperatureUnit DEFAULT_TEMPERATURE_UNIT = TemperatureUnit::Celcius;
    
    const char* as_label(Unit unit);
	const char* as_label(WeightUnit unit);
    const char* as_label(ForceUnit unit);
    const char* as_label(PressureUnit unit);
    const char* as_label(TemperatureUnit unit);
    const char* as_label(TimeUnit unit);
    const char* as_label(QuantityUnit unit);

    const char* as_symbol(Unit unit);
    const char* as_symbol(WeightUnit unit);
    const char* as_symbol(ForceUnit unit);
    const char* as_symbol(PressureUnit unit);
    const char* as_symbol(TemperatureUnit unit);
    const char* as_symbol(TimeUnit unit);
    const char* as_symbol(QuantityUnit unit);

	float getConversionFactor(WeightUnit from, WeightUnit to);
    float getConversionFactor(ForceUnit from, ForceUnit to);
	float getConversionFactor(PressureUnit from, PressureUnit to);
    float getConversionFactor(TimeUnit from, TimeUnit to);
    float getConversionFactor(QuantityUnit from, QuantityUnit to);

    float convert(float value, Unit from, Unit to);
	float convert(float value, WeightUnit from, WeightUnit to);
	float convert(float value, ForceUnit from, ForceUnit to);
	float convert(float value, PressureUnit from, PressureUnit to);
    float convert(float value, TemperatureUnit from, TemperatureUnit to);
    float convert(float value, TimeUnit from, TimeUnit to);
    float convert(float value, QuantityUnit from, QuantityUnit to);
}; // namespace Units