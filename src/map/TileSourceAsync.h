#pragma once

#include "ITileSource.h"
#include "TileAsync.h"

class ITile;
class ITileSaver;

/**
 * @class TileSourceAsync
 * @brief Represents an asynchronous tile source
 */
class TileSourceAsync : public ITileSource {
public:
    TileSourceAsync(int requestLimit, bool preload);
    virtual ~TileSourceAsync();

    virtual bool hasRequest() override;
    virtual bool hasRequest(int z, int x, int y) override;
    virtual bool canRequest() override;
    virtual bool hasFailedManyRequests() override;
    virtual bool request(int z, int x, int y) override;

    virtual void waitAll() override;
    virtual bool canTakeAll() override;
    virtual void takeAll(std::vector<std::shared_ptr<ITile>>& tiles) override;
    virtual int takeReady(std::vector<std::shared_ptr<ITile>>& tiles) override;
    virtual bool saveAll(std::shared_ptr<ITileSaver> saver) override;

    virtual bool preload() const override {
        return _preload;
    }
    virtual int requestLimit() const override {
        return _requestLimit;
    }

    /**
     * @struct TileData
     * @brief The data of a single asynchronously fetched tile
     */
    struct TileData {
        std::atomic_bool& interrupt; ///< Whether to interrupt the fetching of this tile's data
        std::vector<std::byte> blob; ///< The tile's raw data blob
    };

protected:
    /**
     * @brief Fetches the tile at position x,y,z and writes its data in a TileData struct
     * @param z The zoom level of the tile
     * @param x The x position of the tile
     * @param y The y position of the tile
     * @param data The TileData struct in which the received tile data will be writter
     */
    virtual bool receiveTile(int z, int x, int y, TileData& data) = 0;

private:
    TileAsync::FutureData onHandleRequest(int z, int x, int y);

    std::vector<TileAsync> _requests;
    int _requestLimit{10};
    bool _preload{true};
    std::atomic_bool _interrupt;
    int _failedFetchesInARow{};
    int _failedFetchesInARowThreshold{10};
};
