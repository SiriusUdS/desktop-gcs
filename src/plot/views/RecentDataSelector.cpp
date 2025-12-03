#include "RecentDataSelector.h"

RecentDataSelector::RecentDataSelector(size_t timelapseWindow_ms) : timelapseWindow_ms(timelapseWindow_ms) {
}

RecentDataSelector::Window RecentDataSelector::getWindow(const std::vector<float>& timeline) {
    const size_t size = timeline.size();

    if (start >= size) {
        start = 0;
    }

    if (size > 0) {
        const float minX = timeline.back() - timelapseWindow_ms;

        while (start > 0 && timeline.at(start - 1) > minX) {
            start--;
        }

        while (start < size && timeline.at(start) < minX) {
            start++;
        }
    }

    return Window{start, size - start};
}
