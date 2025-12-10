#pragma once

/**
 * @class IRichItem
 * @brief Interface that represents an ImOsm rich item
 */
class IRichItem {
public:
    virtual ~IRichItem() = default;

    /**
     * @brief Check if rich item is in specified bounds
     * @param minLat Bound's min latitude
     * @param maxLat Bound's max latitude
     * @param minLon Bound's min longitude
     * @param maxLon Bound's max longitude
     * @return
     */
    virtual bool inBounds(float minLat, float maxLat, float minLon, float maxLon) const = 0;

    /**
     * @brief Set if the rich item is enabled (shown) or not
     * @param enabled Whether the rich item is to be shown
     */
    virtual void setEnabled(bool enabled) = 0;

    /**
     * @brief Check if rich item is enabled (shown)
     * @return bool
     */
    virtual bool enabled() const = 0;

    /**
     * @brief Paint rich item on ImPlot plot
     */
    virtual void paint() = 0;
};
