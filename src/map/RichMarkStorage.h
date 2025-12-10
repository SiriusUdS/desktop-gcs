#pragma once

#include "Coords.h"

#include <ini.h>
#include <memory>
#include <vector>

class IRichItem;
class MarkItem;

/**
 * @class MarkStorage
 * @brief Storage for marks on the map plot
 */
class MarkStorage {
public:
    MarkStorage();
    ~MarkStorage();

    std::shared_ptr<MarkItem> findMark(const std::string& name) const;
    GeoCoords findMark(const std::string& name, bool& ok) const;

    void loadState(const mINI::INIStructure& ini);
    void saveState(mINI::INIStructure& ini) const;

    void addMark(const GeoCoords& coords, const std::string& name);
    void rmMarks();

    struct ItemNode;

    /**
     * @brief Returns the mark items in the storage
     * @returns Vector containing the mark items
     */
    inline std::vector<ItemNode>& markItems() {
        return _markItems;
    }

    /**
     * @brief Returns the mark items in the storage (const)
     * @returns Vector containing the mark items
     */
    inline const std::vector<ItemNode>& markItems() const {
        return _markItems;
    }

public:
    /**
     * @struct ItemNode
     * @brief Represents a mark item in storage
     */
    struct ItemNode {
        std::shared_ptr<MarkItem> ptr; ///< Shared pointer to mark item
        mutable bool rmFlag{false};    ///< Flag that dictates whether the mark item can be deleted
    };

private:
    // Mark Items
    std::vector<ItemNode> _markItems;
};
