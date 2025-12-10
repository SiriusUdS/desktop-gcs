#pragma once

#include <imgui.h>

/**
 * @class ITile
 * @brief Interface that represents an ImOsm tile
 */
class ITile {
public:
    virtual ~ITile() = default;

    virtual int z() const = 0; ///< Zoom level of the tile
    virtual int x() const = 0; ///< X position of tile at current zoom level
    virtual int y() const = 0; ///< Y position of tile at current zoom level

    /**
     * @brief Checks if the tile's ZXY values correspond to the ones passed as parameters
     * @param z Zoom level
     * @param x X position
     * @param y Y position
     * @return bool
     */
    virtual bool isTileZXY(int z, int x, int y) const = 0;

    /**
     * @brief Checks if the tile is in the specified bounds at a certain zoom level
     * @param z Zoom level
     * @param xmin Bound's min x
     * @param xmax Bound's max x
     * @param ymin Bound's min y
     * @param ymax Bound's max y
     * @return bool
     */
    virtual bool inBounds(int z, int xmin, int xmax, int ymin, int ymax) const = 0;

    /**
     * @brief Checks if the tile is a dummy, meaning it doesn't contain any image data
     * @return bool
     */
    virtual bool isDummy() const = 0;

    /**
     * @brief Raw image blob
     * @return const char*
     */
    virtual const char* rawBlob() const = 0;

    /**
     * @brief Size of raw image blob
     * @return size_t
     */
    virtual size_t rawBlobSize() const = 0;

    /**
     * @brief Loads rgba image data from rawBlob
     */
    virtual void rgbaLoad() const = 0;

    /**
     * @brief rgba image blob
     * @return const char*
     */
    virtual const char* rgbaBlob() const = 0;

    /**
     * @brief Size of rgba image blob
     * @return size_t
     */
    virtual size_t rgbaBlobSize() const = 0;

    /**
     * @brief ImGui texture id of tile
     * @return ImTextureID
     */
    virtual ImTextureID texture() const = 0;
};
