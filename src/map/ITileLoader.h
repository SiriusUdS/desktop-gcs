#pragma once

#include <imgui.h>

/**
 * @class ITileLoader
 * @brief Interface that represents an ImOsm tile loader
 */
class ITileLoader {
public:
    virtual ~ITileLoader() = default;

    /**
     * @brief Begin loading tiles in specified bounds and zoom level
     * @param z Zoom level
     * @param xmin Bound's min x
     * @param xmax Bound's max x
     * @param ymin Bound's min y
     * @param ymax Bound's max y
     */
    virtual void beginLoad(int z, int xmin, int xmax, int ymin, int ymax) {
    }

    /**
     * @brief Fetch the ImGui texture id of a specific tile
     * @param z Zoom level
     * @param x X position
     * @param y Y position
     * @return ImTextureID
     */
    virtual ImTextureID tileAt(int z, int x, int y) = 0;

    /**
     * @brief Number of tiles loaded
     * @return int
     */
    virtual int tileCount() const {
        return 0;
    };

    /**
     * @brief Stop loading tiles
     */
    virtual void endLoad() {
    }

    /**
     * @brief Check if load has failed
     * @return bool
     */
    virtual bool failedLoad() {
        return false;
    }
};
