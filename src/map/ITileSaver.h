#pragma once

#include <memory>
#include <vector>

class ITile;

/**
 * @class ITileSaver
 * @brief Interface that represents an ImOsm tile saver
 */
class ITileSaver {
public:
    virtual ~ITileSaver() = default;

    /**
     * @brief Save a single tile
     * @return bool Tile was saved successfully
     */
    virtual bool save(std::shared_ptr<ITile> tile) const = 0;

    /**
     * @brief Save multiple tile
     * @return bool All tiles were saved successfully
     */
    virtual bool saveMulti(const std::vector<std::shared_ptr<ITile>>& tiles) const = 0;
};
