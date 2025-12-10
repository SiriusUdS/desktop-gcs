#pragma once

#include "ITileSaver.h"
#include "TileSourceFs.h"

#include <filesystem>

class ITile;

/**
 * @class TileSaver
 * @brief Base tile saver
 */
class TileSaver : public ITileSaver {
public:
    TileSaver();
    virtual ~TileSaver();

    virtual bool saveMulti(const std::vector<std::shared_ptr<ITile>>& tiles) const override;
    virtual bool save(std::shared_ptr<ITile> tile) const override;

protected:
    /**
     * @brief Get directory path for tile to be saved at
     * @return std::filesystem::path
     */
    virtual std::filesystem::path dirPath(std::shared_ptr<ITile>) const = 0;
    virtual std::string fileName(std::shared_ptr<ITile> tile) const;
};

/**
 * @class TileSaverDir
 * @brief Saves tiles in a specific directory
 */
class TileSaverDir : public TileSaver {
public:
    TileSaverDir();
    TileSaverDir(const std::filesystem::path& basePath);
    virtual ~TileSaverDir();

protected:
    virtual std::filesystem::path dirPath(std::shared_ptr<ITile> tile) const override;

private:
    std::filesystem::path _basePath{TileSourceFs::BasePathDefault()};
};

/**
 * @class TileSaverSubDir
 * @brief Saves tiles in a subdirectory of a specific directory
 */
class TileSaverSubDir : public TileSaver {
public:
    TileSaverSubDir();
    TileSaverSubDir(const std::filesystem::path& basePath);
    virtual ~TileSaverSubDir();

protected:
    virtual std::filesystem::path dirPath(std::shared_ptr<ITile> tile) const override;

private:
    std::filesystem::path _basePath{TileSourceFs::BasePathDefault()};
};
