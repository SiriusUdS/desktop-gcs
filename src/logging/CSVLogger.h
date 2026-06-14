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

    // Flushing every row throttles the receive/decode thread enough to drop
    // packets at the ~2 kHz telemetry rate, so flush in batches instead: data is
    // still on disk within FLUSH_INTERVAL rows, but the high-rate stream keeps up.
    static constexpr size_t FLUSH_INTERVAL = 256;

    std::vector<Column> columns;
    std::ofstream file;
    std::string currentFileName;
    bool firstLog{true};
    size_t rowsSinceFlush{};
};
