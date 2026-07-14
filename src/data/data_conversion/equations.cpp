#include "equations.h"
#include <filesystem>
#include <fstream>

using namespace equations;

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

const std::vector<std::string> equations::EquationHandler::getKeys() {
   std::vector<std::string> keys;
    for (const auto& [key, _] : equationsFile) {
        keys.push_back(key);
    }

    return keys;
}

equations::EquationHandler::~EquationHandler() {
    saveData();
}
