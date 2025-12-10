#pragma once

#include "ITileLoader.h"

#include <memory>
#include <vector>

class ITile;
class ITileSource;

/**
 * @class TileLoader
 * @brief Loads tiles from a tile source
 */
class TileLoader : public ITileLoader {
public:
    TileLoader(std::shared_ptr<ITileSource> source, bool dummiesAreFinal = false);
    virtual ~TileLoader() = default;

    virtual void beginLoad(int z, int xmin, int xmax, int ymin, int ymax) override;
    virtual ImTextureID tileAt(int z, int x, int y) override;
    virtual int tileCount() const override {
        return (int) _tiles.size();
    }
    virtual void endLoad() override {
    }
    virtual bool failedLoad() override;

private:
    std::shared_ptr<ITileSource> _source;
    std::vector<std::shared_ptr<ITile>> _tiles;
    bool _dummiesAreFinal{};
};
