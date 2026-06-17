#include "RecentDataSelector.h"

RecentDataSelector::RecentDataSelector(float duration, Units::TimeUnit durationUnit): duration(duration), durationUnit(durationUnit) {}

DataSelector::Window
RecentDataSelector::getWindow(const PlotView& view) {
    const auto& timeline = view.timeline;
    const size_t size = timeline.size();

    if (size == 0) {
        return {0, 0};
    }

    if (start >= size) {
        start = 0;
    }

    const auto timelineUnit = std::get<Units::TimeUnit>(view.timelineUnit);

    const float durationInTimelineUnits = Units::convert(duration, durationUnit, timelineUnit);

    const float minX = timeline.back() - durationInTimelineUnits;

    while (start > 0 && timeline[start - 1] > minX) {
        --start;
    }

    while (start < size && timeline[start] < minX) {
        ++start;
    }

    return {start, size - start};
}