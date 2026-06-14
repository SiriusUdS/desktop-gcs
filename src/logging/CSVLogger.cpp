#include "CSVLogger.h"

#include "Logging.h"

#include <iomanip>
#include <sstream>
#include <string>

namespace {
// Render a value in plain decimal notation (never scientific), trimming trailing
// zeros so whole numbers like ADC counts log as "1234567" rather than "1.23457e+06".
std::string formatValue(float value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    std::string s = oss.str();
    if (s.find('.') != std::string::npos) {
        s.erase(s.find_last_not_of('0') + 1); // drop trailing zeros
        if (s.back() == '.') {
            s.pop_back(); // drop a now-dangling decimal point
        }
    }
    return s;
}
} // namespace

/**
 * @brief Opens the file in which the CSV data will be logged.
 * @param filename The name of the file to open.
 */
void CSVLogger::openFile(std::string filename) {
    if (file.is_open()) {
        file.close();
    }

    file.open(filename, std::ios::out);
    if (!file.is_open()) {
        GCS_APP_LOG_ERROR("CSVLogger: Couldn't open file \"{}\".", filename);
    }

    firstLog = true;
    rowsSinceFlush = 0;
    currentFileName = filename;
}

/**
 * @brief Adds a new column to the CSV file.
 * @param column The name of the column to add.
 */
void CSVLogger::addColumn(std::string column) {
    columns.emplace_back(column, 0.f);
}

/**
 * @brief Sets the value of a specific column.
 * @param columnIndex The index of the column to set the value for.
 * @param value The value to set for the column.
 */
void CSVLogger::setValue(size_t columnIndex, float value) {
    if (columnIndex >= columns.size()) {
        GCS_APP_LOG_ERROR("CSVLogger: Couldn't set column value, column index ({}) out of bounds.", columnIndex);
        return;
    }

    columns[columnIndex].currentValue = value;
}

/**
 * @brief Logs the current values of all columns to the CSV file.
 * If the file is not open or no columns have been added, it logs an error or warning.
 */
void CSVLogger::log() {
    if (!file.is_open()) {
        GCS_APP_LOG_ERROR("CSVLogger: Can't log in file \"{}\", file isn't open.", currentFileName);
        return;
    }

    if (columns.empty()) {
        GCS_APP_LOG_WARN("CSVLogger: Tried to log in file \"{}\", but no columns have been created.", currentFileName);
        return;
    }

    if (firstLog) {
        for (size_t i = 0; i < columns.size() - 1; i++) {
            file << columns[i].name << ",";
        }
        file << columns.back().name << '\n';
        firstLog = false;
    }

    for (size_t i = 0; i < columns.size() - 1; i++) {
        file << formatValue(columns[i].currentValue) << ",";
    }
    file << formatValue(columns.back().currentValue) << '\n';
    firstLog = false;

    if (++rowsSinceFlush >= FLUSH_INTERVAL) {
        file.flush();
        rowsSinceFlush = 0;
    }
}

/**
 * @brief Checks if the CSV file is currently open.
 * @returns True if the file is open, false otherwise.
 */
bool CSVLogger::fileIsOpen() const {
    return file.is_open();
}
