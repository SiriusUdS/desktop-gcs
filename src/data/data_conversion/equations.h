#pragma once
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace equations {
inline constexpr std::string_view EquationFilePath = "equations.bin";
class EquationHandler {
private:
    std::map<std::string, std::string> equationsFile;
    EquationHandler() = default;
    void loadData();
    void saveData();

public:
    static EquationHandler& getInstance();

    EquationHandler(const EquationHandler&) = delete;
    EquationHandler& operator=(const EquationHandler&) = delete;

    void setEquation(const std::string& name, const std::string& equation);

    std::string getEquation(const std::string& name);

    const std::vector<std::string> getKeys();

    ~EquationHandler();
};
} // namespace equations