#pragma once

#include <memory>
#include <vector>

class ITile;
class ITileSaver;

/**
 * @class ITileSource
 * @brief Interface that represents an ImOsm tile source
 */
class ITileSource {
public:
    virtual ~ITileSource() = default;

    /**
     * @brief Check if there are any request in progress
     * @return bool
     */
    virtual bool hasRequest() = 0;

    /**
     * @brief Check if there's a request in progress for a specific tile
     * @param z Zoom level
     * @param x X position
     * @param y Y position
     * @return bool
     */
    virtual bool hasRequest(int z, int x, int y) = 0;

    /**
     * @brief Check if more requests can be made
     * @return bool
     */
    virtual bool canRequest() = 0;

    /**
     * @brief Check if the number of failed requests has exceeded a certain threshold
     * @return bool
     */
    virtual bool hasFailedManyRequests() = 0;

    /**
     * @brief Request a specific tile from source
     * @return bool Request has been successfully created
     */
    virtual bool request(int z, int x, int y) = 0;

    /**
     * @brief Wait for all requests to be done
     */
    virtual void waitAll() = 0;

    /**
     * @brief Check if all requests are done
     * @return bool
     */
    virtual bool canTakeAll() = 0;

    /**
     * @brief Fetch tile data from every request
     */
    virtual void takeAll(std::vector<std::shared_ptr<ITile>>& tiles) = 0;

    /**
     * @brief Fetch tile data from completed requests
     */
    virtual int takeReady(std::vector<std::shared_ptr<ITile>>& tiles) = 0;

    /**
     * @brief Save all tile data with specified tile saver
     * @return bool
     */
    virtual bool saveAll(std::shared_ptr<ITileSaver> saver) = 0;

    /**
     * @brief Check if tile image data should be loaded immediately upon tile object creation
     * @return bool
     */
    virtual bool preload() const = 0;

    /**
     * @brief Maximum amount of requests that can be made at the same time
     * @return int
     */
    virtual int requestLimit() const = 0;
};
