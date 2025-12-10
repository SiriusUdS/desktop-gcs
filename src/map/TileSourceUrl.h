#pragma once

#include "TileSourceAsync.h"

/**
 * @class TileSourceUrl
 * @brief Tile source that fetches tiles from a url
 */
class TileSourceUrl : public TileSourceAsync {
public:
    TileSourceUrl(int requestLimit, bool preload, const std::string& userAgent = "curl");
    virtual ~TileSourceUrl();

protected:
    virtual bool receiveTile(int z, int x, int y, TileData& tileData) override;

    /**
     * @brief Generates the url to fetch a specific tile
     * @param z Zoom level of the tile
     * @param x X position of the tile
     * @param y Y position of the tile
     */
    virtual std::string makeUrl(int z, int x, int y) = 0;

private:
    std::string _userAgent{"curl"};
};
