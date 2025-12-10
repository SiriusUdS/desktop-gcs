#pragma once

#include <vector>

class PlotRawData;

namespace PlotDataCompression {
void meanCompression(const std::vector<float>& originalData, std::vector<float>& compressedData, size_t targetCompressionSize);
} // namespace PlotDataCompression
