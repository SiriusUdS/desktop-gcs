#ifndef RECENTDATASELECTOR_H
#define RECENTDATASELECTOR_H

#include "DataSelector.h"

class RecentDataSelector : public DataSelector {
public:
    RecentDataSelector(size_t timelapseWindow_ms);
    Window getWindow(const std::vector<float>& timeline) override;

private:
    size_t timelapseWindow_ms{};
    size_t start{};
};

#endif // RECENTDATASELECTOR_H
