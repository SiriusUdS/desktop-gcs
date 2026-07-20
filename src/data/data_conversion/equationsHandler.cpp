#include "equationHandler.h"

#include <filesystem>
#include <fstream>

using namespace equations;

#include <cctype>
#include <cstdlib>
#include <string>

// Helper class to evaluate math expressions
class MathInterpreter {
    const char* p;
    float x_val;
    //log approximation for IEEE 754 floats
    float deterministic_ln(float x) {
        uint32_t i;
        std::memcpy(&i, &x, sizeof(float));

        int32_t biased_exp = static_cast<int32_t>((i >> 23) & 0xFF);
        float exp = static_cast<float>(biased_exp - 127);

        uint32_t mantissa_bits = (i & 0x7FFFFF) | 0x3F800000;
        float m;
        std::memcpy(&m, &mantissa_bits, sizeof(float)); // m in [1, 2)

        // Range-reduce m into [sqrt(2)/2, sqrt(2)] so the series argument is small
        constexpr float SQRT2 = 1.41421356f;
        if (m > SQRT2) {
            m *= 0.5f;
            exp += 1.0f;
        }

        // atanh-based series: ln(m) = 2*atanh(y) = 2*(y + y^3/3 + y^5/5 + y^7/9 + ...)
        float y = (m - 1.0f) / (m + 1.0f);
        float y2 = y * y;
        float series = 1.0f + y2 * (1.0f / 3.0f + y2 * (1.0f / 5.0f + y2 * (1.0f / 7.0f + y2 * (1.0f / 9.0f))));
        float ln_m = 2.0f * y * series;

        return exp * 0.69314718f + ln_m;
    }

    // Higher-order minimax approx for 2^f
    float deterministic_exp(float x) {
        x = std::fmax(-87.0f, std::fmin(88.0f, x));

        constexpr float log2e = 1.4426950f;
        float t = x * log2e;
        float fi = std::floor(t);
        float f = t - fi;

        // 5th-order minimax for 2^f, f in [0,1)
        float p2f = 1.0f + f * (0.6931472f + f * (0.2401795f + f * (0.0555058f + f * (0.0096180f + f * 0.0013693f))));

        int32_t exp_bits = (static_cast<int32_t>(fi) + 127) << 23;
        float scale;
        std::memcpy(&scale, &exp_bits, sizeof(float));

        return p2f * scale;
    }

    // Exact integer power via exponentiation by squaring — O(log n), no approximation error
    float integerPow(float base, int32_t n) {
        bool negExp = n < 0;
        uint32_t un = negExp ? static_cast<uint32_t>(-static_cast<int64_t>(n)) : static_cast<uint32_t>(n);
        float result = 1.0f;
        float b = base;
        while (un > 0) {
            if (un & 1)
                result *= b;
            b *= b;
            un >>= 1;
        }
        return negExp ? (1.0f / result) : result;
    }

    float deterministic_pow(float base, float exponent) {
        // Fast, exact path: integer (or near-integer) exponents
        float rounded = std::round(exponent);
        if (std::fabs(exponent - rounded) < 1e-6f) {
            return integerPow(base, static_cast<int32_t>(rounded));
        }

        // Fractional exponent: fall back to log/exp
        if (base == 0.0f)
            return 0.0f;
        if (base < 0.0f)
            return std::numeric_limits<float>::quiet_NaN(); // undefined for real result

        return deterministic_exp(exponent * deterministic_ln(base));
    }

    

    // Recursive Descent Parsing
    float parseExpression() {
        float result = parseTerm();
        while (*p == '+' || *p == '-') {
            if (*p == '+') {
                p++;
                result += parseTerm();
            } else if (*p == '-') {
                p++;
                result -= parseTerm();
            }
        }
        return result;
    }

    float parseTerm() {
        float result = parseFactor();
        while (*p == '*' || *p == '/') {
            if (*p == '*') {
                p++;
                result *= parseFactor();
            } else if (*p == '/') {
                p++;
                result /= parseFactor();
            }
        }
        return result;
    }

    float parseFactor() {
        // Handle unary plus/minus
        if (*p == '+') {
            p++;
            return parseFactor();
        }
        if (*p == '-') {
            p++;
            return -parseFactor();
        }

        float result;

        // Handle 'ln(' function call
        if ((*p == 'l' || *p == 'L') && (*(p + 1) == 'n' || *(p + 1) == 'N') && *(p + 2) == '(') {
            p += 3;
            result = deterministic_ln(parseExpression());
            if (*p == ')')
                p++;
        }
        // Handle Parentheses
        else if (*p == '(') {
            p++;
            result = parseExpression();
            if (*p == ')')
                p++;
        }
        // Handle Variable 'x'
        else if (*p == 'x' || *p == 'X') {
            p++;
            result = x_val;
        }
        // Handle Numbers
        else {
            char* next;
            result = std::strtof(p, &next);
            p = next;
        }

        // Handle exponent '^' — right-associative, recurses back into parseFactor
        // so it picks up unary minus, parens, ln(), nested '^', etc. for free
        if (*p == '^') {
            p++;
            float exponent = parseFactor();
            result = deterministic_pow(result, exponent);
        }

        return result;
    }


public:
    MathInterpreter(const char* str, float x) : p(str), x_val(x) {
    }

    float evaluate() {
        return parseExpression();
    }
};

// Pre-processor to insert explicit '*' for implicit multiplication
std::string preprocessEquation(const std::string& eq) {
    std::string res;
    for (size_t i = 0; i < eq.length(); ++i) {
        char c = eq[i];
        if (std::isspace(c))
            continue; // Strip spaces

        if (!res.empty()) {
            char prev = res.back();
            bool isPrevDigit = std::isdigit(prev) || prev == '.';
            bool isPrevVar = (prev == 'x' || prev == 'X');
            bool isPrevClose = (prev == ')');

            bool isCurrVar = (c == 'x' || c == 'X');
            bool isCurrOpen = (c == '(');
            bool isCurrDigit = std::isdigit(c) || c == '.';

            // Insert '*' if we see patterns like: '2x', 'x2', '2(', ')2', ')x', or ')('
            if ((isPrevDigit && (isCurrVar || isCurrOpen)) || (isPrevVar && (isCurrDigit || isCurrOpen))
                || (isPrevClose && (isCurrDigit || isCurrVar || isCurrOpen))) {
                res += '*';
            }
        }
        res += c;
    }
    return res;
}

EquationHandler::EquationHandler() {
    loadData();
}

void equations::EquationHandler::loadData() {
    std::ifstream iFile(std::filesystem::path(EquationFilePath), std::ios::binary);

    if (!iFile)
        return;

    size_t count;

    iFile.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; i++) {
        size_t nameSize;

        iFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));

        std::string name(nameSize, '\0');

        iFile.read(name.data(), nameSize);

        size_t equationSize;

        iFile.read(reinterpret_cast<char*>(&equationSize), sizeof(equationSize));

        std::string equation(equationSize, '\0');

        iFile.read(equation.data(), equationSize);

        equationsFile[name] = equation;
    }
}

void equations::EquationHandler::saveData() {
    if (equationsFile.empty())
        return;

    std::ofstream oFile(std::filesystem::path(EquationFilePath), std::ios::binary);

    if (!oFile)
        return;

    // Write number of equations
    size_t count = equationsFile.size();

    oFile.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [name, equation] : equationsFile) {
        // Write name length
        size_t nameSize = name.size();

        oFile.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));

        // Write name data
        oFile.write(name.data(), nameSize);

        // Write equation length
        size_t equationSize = equation.size();

        oFile.write(reinterpret_cast<const char*>(&equationSize), sizeof(equationSize));

        // Write equation data
        oFile.write(equation.data(), equationSize);
    }
}

EquationHandler& EquationHandler::getInstance() {
    static EquationHandler handler;
    return handler;
}

void EquationHandler::setEquation(const std::string& name, const std::string& equation) {
    equationsFile[name] = equation;
}


std::string EquationHandler::getEquation(const std::string& name) {
    auto it = equationsFile.find(name);

    if (it != equationsFile.end())
        return it->second;

    return "";
}

void equations::EquationHandler::removeEquation(const std::string& name) {
    equationsFile.erase(name);
}

float equations::EquationHandler::evaluate(const std::string& name, const float x) {
    std::string eq = getEquation(name);
    if (eq == "") {
        return 0.0f;
    }

    std::string processedEq = preprocessEquation(eq);

    // 2. Parse and evaluate
    MathInterpreter interpreter(processedEq.c_str(), x);
    return interpreter.evaluate();
}

std::vector<std::string> EquationHandler::getKeys() {
    std::vector<std::string> keys;
    for (const auto& [key, _] : equationsFile) {
        keys.push_back(key);
    }

    return keys;
}

equations::EquationHandler::~EquationHandler() {
    saveData();
}
