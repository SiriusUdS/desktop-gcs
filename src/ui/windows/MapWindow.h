#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include "RichMapPlot.h"
#include "RichMarkStorage.h"
#include "TileGrabber.h"
#include "TileSourceUrlConnTest.h"
#include "TileSourceUrlImpl.h"
#include "UIWindow.h"

#include <chrono>

class MapWindow : public UIWindow {
public:
    void init() override;
    void render() override;
    void loadState(const mINI::INIStructure& ini) override;
    void saveState(mINI::INIStructure& ini) const override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    enum MapView { MAP_VIEW = 0, SATELLITE_VIEW = 1 };

    void addMark(const GeoCoords& coords, const std::string& name);
    std::string getFsPathFromMapView(int mapView) const;
    void startTileProviderConnectivityTest();
    void updateMarkStyle();

    const char* INI_MAP_WINDOW_MAP_VIEW = "map_window_map_view";

    int mapView{};
    int prevMapView{};
    bool sourceIsFs{};
    bool canFetchTilesFromUrl{false};
    int downloadMinZ{0};
    int downloadMaxZ{18};
    size_t downloadTileCount{0};
    size_t downloadTileTotal{};
    float downloadProgress{0};
    std::chrono::seconds autoSourceSwitchDelay{1};
    std::chrono::steady_clock::time_point lastAutoSourceSwitchTime{std::chrono::steady_clock::now()};
    std::shared_ptr<RichMapPlot> mapPlot;
    std::shared_ptr<MarkStorage> storage;
    std::shared_ptr<TileGrabber> mapTileGrabber;
    std::shared_ptr<TileGrabber> satelliteTileGrabber;
    std::shared_ptr<TileSourceUrlConnTest> urlConnectionTest;
};

#endif // MAPWINDOW_H
