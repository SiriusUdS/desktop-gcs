#pragma once

#include "units.h"

#include <vector>

struct PlotView {
    std::vector<float> timeline;
    std::vector<float> values;

    Units::Unit timelineUnit;
    Units::Unit valueUnit;
};