#ifndef PLOTTIMELINE_H
#define PLOTTIMELINE_H

#include "DataSeries.h"

class PlotTimeline : public DataSeries {
public:
    PlotTimeline(size_t targetCompressionSize);
    void add(float timestamp) override;
    void clear() override;

    bool isUnordered() const;

private:
    bool dataUnordered{};
};

#endif // PLOTTIMELINE_H
