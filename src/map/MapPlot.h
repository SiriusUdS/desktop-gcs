#pragma once

#include <memory>

class ITileLoader;

/**
 * @class MapPlot
 * @brief The ImGui map plot component
 */
class MapPlot {
public:
    MapPlot();
    MapPlot(std::shared_ptr<ITileLoader>& loader);
    virtual ~MapPlot();

    /**
     * @brief Set the map plot's tile loader.
     * @param loader The new tile loader
     */
    inline void setTileLoader(std::shared_ptr<ITileLoader> loader) {
        _loader = loader;
    }

    virtual void paint();

    /**
     * @brief Paints additionnal elements over the map
     */
    virtual void paintOverMap() {
    }

    inline void resetBounds();

    // Geo CS routines
    void setBoundsGeo(float minLat, float maxLat, float minLon, float maxLon);
    void getBoundsGeo(float& minLat, float& maxLat, float& minLon, float& maxLon) const;

    /**
     * @brief Returns the minimum latitude of the displayed map bounds.
     */
    inline float minLat() const {
        return _minLat;
    }

    /**
     * @brief Returns the maximum latitude of the displayed map bounds.
     */
    inline float maxLat() const {
        return _maxLat;
    }

    /**
     * @brief Returns the minimum longitude of the displayed map bounds.
     */
    inline float minLon() const {
        return _minLon;
    }

    /**
     * @brief Returns the maximum longitude of the displayed map bounds.
     */
    inline float maxLon() const {
        return _maxLon;
    }

    /**
     * @brief Returns the latitude of the mouse's position on the map.
     */
    inline float mouseLat() const {
        return _mouseLat;
    }

    /**
     * @brief Returns the longitude of the mouse's position on the map.
     */
    inline float mouseLon() const {
        return _mouseLon;
    }

    /**
     * @brief Returns true if the latitude/longitude point passed in parameters is within the displayed map's bounds.
     */
    inline bool inBoundsGeo(float lat, float lon) const {
        return lat > _minLat && lat < _maxLat && lon > _minLon && lon < _maxLon;
    }

    // Tile CS routines
    void getBoundsTile(int& minTX, int& maxTX, int& minTY, int& maxTY) const;

    /**
     * @brief Returns the the x position of the leftmost tile in the displayed map's bounds.
     */
    inline int minTileX() const {
        return _minTX;
    };

    /**
     * @brief Returns the the x position of the rightmost tile in the displayed map's bounds.
     */
    inline int maxTileX() const {
        return _maxTX;
    };

    /**
     * @brief Returns the the y position of the upmost tile in the displayed map's bounds.
     */
    inline int minTileY() const {
        return _minTY;
    };

    /**
     * @brief Returns the the y position of the downmost tile in the displayed map's bounds.
     */
    inline int maxTileY() const {
        return _maxTY;
    };

    /**
     * @brief Returns the map's zoom level.
     */
    inline int zoom() const {
        return _zoom;
    }

    // Local CS routines
    void setBoundsLocal(float minX, float maxX, float minY, float maxY);
    void getBoundsLocal(float& minX, float& maxX, float& minY, float& maxY) const;

    /**
     * @brief Returns the minimum local x of the displayed map bounds.
     */
    inline int minLocalX() const {
        return (int) _minX;
    };

    /**
     * @brief Returns the maximum local x of the displayed map bounds.
     */
    inline int maxLocalX() const {
        return (int) _maxX;
    };

    /**
     * @brief Returns the minimum local y of the displayed map bounds.
     */
    inline int minLocalY() const {
        return (int) _minY;
    };

    /**
     * @brief Returns the maximum local y of the displayed map bounds.
     */
    inline int maxLocalY() const {
        return (int) _maxY;
    };

    /**
     * @brief Returns true if the x/y point is within the display map's local bounds.
     */
    inline bool inBoundsLocal(float x, float y) const {
        return x > _minX && x < _maxX && y > _minY && y < _maxY;
    }

    bool mouseOnPlot() const;

    bool failedToFetchTiles() const;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
    std::shared_ptr<ITileLoader> _loader, _prevLoader;

    float _tilePixels{256.0};
    float _tileSize{};
    float _minLat{}, _maxLat{};
    float _minLon{}, _maxLon{};
    float _minX{}, _maxX{};
    float _minY{}, _maxY{};
    int _minTX{}, _maxTX{};
    int _minTY{}, _maxTY{};
    int _zoom{};
    float _pixelsX{}, _pixelsY{};
    float _rangeX{}, _rangeY{};
    float _resX{}, _resY{};
    int _tilesNum{};
    float _mouseLat{}, _mouseLon{};
    float _mouseClickedLat{}, _mouseClickedLon{};

    enum class SetBounds { None, Geo, Local };
    SetBounds _setBounds{SetBounds::None};
};
