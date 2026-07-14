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
        // Handle unary plus/minus (e.g., the negative in "-10x")
        if (*p == '+') {
            p++;
            return parseFactor();
        }
        if (*p == '-') {
            p++;
            return -parseFactor();
        }

        // Handle Parentheses
        if (*p == '(') {
            p++;
            float result = parseExpression();
            if (*p == ')')
                p++; // consume closing paren
            return result;
        }

        // Handle Variable 'x'
        if (*p == 'x' || *p == 'X') {
            p++;
            return x_val;
        }

        // Handle Numbers
        char* next;
        float val = std::strtof(p, &next);
        p = next;
        return val;
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
