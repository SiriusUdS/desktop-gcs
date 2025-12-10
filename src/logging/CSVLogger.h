#pragma once

#include <fstream>
#include <vector>

/**
 * @class CSVLogger
 * @brief Logs data in files in the CSV format.
 */
class CSVLogger {
public:
    void openFile(std::string filename);
    void addColumn(std::string column);
    void setValue(size_t columnIndex, float value);
    void log();
    bool fileIsOpen() const;

private:
    struct Column {
        std::string name;
        float currentValue;
    };

    std::vector<Column> columns;
    std::ofstream file;
    std::string currentFileName;
    bool firstLog{true};
};
