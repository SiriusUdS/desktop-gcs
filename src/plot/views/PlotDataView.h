#ifndef PLOTDATAVIEW_H
#define PLOTDATAVIEW_H

#include "DataSelector.h"

class PlotData;
struct ThemedColor;

class PlotDataView {
public:
    PlotDataView(const PlotData& plotData);
    PlotDataView(const PlotData& plotData, const ThemedColor& colorOverride);
    void plot(bool showCompressedData);

protected:
    virtual DataSelector::Window getDataSelectorWindow(bool getCompressedData, const std::vector<float>& timeline) = 0;

private:
    const PlotData& plotData;
    const ThemedColor& color;
};

#endif // PLOTDATAVIEW_H
