#ifndef PLOTVALUES_H
#define PLOTTIMELINE_H

#include "DataSeries.h"

class PlotValues : public DataSeries {
public:
    void add(float value) override;
    void clear() override;
};

#endif // PLOTTIMELINE_H
