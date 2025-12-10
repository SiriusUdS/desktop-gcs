#pragma once

#include "MapPlot.h"

#include <ini.h>
#include <memory>
#include <vector>

class IRichItem;

/**
 * @class RichMapPlot
 * @brief The ImGui map plot component with additional features
 */
class RichMapPlot : public MapPlot {
public:
    RichMapPlot();
    virtual ~RichMapPlot() override;

    virtual void paintOverMap() override;
    void loadState(const mINI::INIStructure& ini);
    void saveState(mINI::INIStructure& ini) const;
    void addItem(std::weak_ptr<IRichItem> item);

private:
    std::vector<std::weak_ptr<IRichItem>> _items;
};
