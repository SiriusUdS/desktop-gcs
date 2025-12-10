#pragma once

#include "TileLoader.h"
#include "TileSourceFs.h"
#include "TileSourceUrlImpl.h"

#include <memory>

inline constexpr int URL_REQUEST_LIMIT{10};
inline constexpr int FS_REQUEST_LIMIT{10};
inline bool MAP_PRELOAD{true};

/**
 * @class TileLoaderOsmMap
 * @brief Loads tiles from an Open Street Map (map tiles) tile source
 */
class TileLoaderOsmMap : public TileLoader {
public:
    /**
     * @brief Constructs an OSM map tile loader
     * @param requestLimit Maximum number of concurrent requests that can be made to load tiles
     */
    TileLoaderOsmMap(const int requestLimit = URL_REQUEST_LIMIT) : TileLoader{std::make_shared<TileSourceUrlOsm>(requestLimit, MAP_PRELOAD)} {
    }
};

/**
 * @class TileLoaderArcMap
 * @brief Loads tiles from an Arc Imagery (satellite tiles) tile source
 */
class TileLoaderArcMap : public TileLoader {
public:
    /**
     * @brief Constructs an Arc map tile loader
     * @param requestLimit Maximum number of concurrent requests that can be made to load tiles
     */
    TileLoaderArcMap(const int requestLimit = URL_REQUEST_LIMIT) : TileLoader{std::make_shared<TileSourceUrlArc>(requestLimit, MAP_PRELOAD)} {
    }
};

/**
 * @class TileLoaderUrlMap
 * @brief Loads tiles from an url map tile source
 */
class TileLoaderUrlMap : public TileLoader {
public:
    /**
     * @brief Constructs an url map tile loader
     * @param url_tpl The url from which the tiles will be loaded from
     * @param requestLimit Maximum number of concurrent requests that can be made to load tiles
     */
    TileLoaderUrlMap(const std::string& url_tpl, const int requestLimit = URL_REQUEST_LIMIT)
        : TileLoader{std::make_shared<TileSourceUrlCustom>(requestLimit, MAP_PRELOAD, url_tpl)} {
    }
};

/**
 * @class TileLoaderFsMap
 * @brief Loads tiles from a file system tile source
 */
class TileLoaderFsMap : public TileLoader {
public:
    /**
     * @brief Constructs a file system map tile loader
     * @param dirname The name of the folder from which the tiles will be loaded from
     * @param requestLimit Maximum number of concurrent requests that can be made to load tiles
     */
    TileLoaderFsMap(const std::string& dirname, const int requestLimit = URL_REQUEST_LIMIT)
        : TileLoader{std::make_shared<TileSourceFsSubDir>(requestLimit, MAP_PRELOAD, dirname), true} {
    }
};
