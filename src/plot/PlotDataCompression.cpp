#include "PlotDataCompression.h"

#include "DataSeries.h"
#include "Logging.h"

void PlotDataCompression::meanCompression(const std::vector<float>& originalData, std::vector<float>& compressedData, size_t targetCompressionSize) {
    const size_t dataSize = originalData.size();
    const size_t compressedDataSize = compressedData.size();

    if (dataSize <= targetCompressionSize || targetCompressionSize == 0) {
        compressedData = originalData;
        return;
    }

    const double averageBinSize = dataSize / (double) targetCompressionSize;
    double binAccumulator = 0;
    size_t currentBinSize = 0;

    compressedData.clear();

    for (size_t i = 0; i < dataSize; i += currentBinSize) {
        binAccumulator += averageBinSize;
        currentBinSize = (size_t) binAccumulator;
        binAccumulator -= currentBinSize;

        float sum = 0;
        size_t end = std::min(i + currentBinSize, dataSize);
        size_t count = 0;

        for (size_t j = i; j < end; j++) {
            sum += originalData.at(j);
            count++;
        }

        if (count > 0) {
            compressedData.push_back(sum / (float) count);
        }
    }
}
