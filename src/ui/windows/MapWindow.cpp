#include "MapWindow.h"

#include "IniConfig.h"
#include "Logging.h"
#include "RichMarkItem.h"
#include "TileLoaderImpl.h"
#include "TileSaver.h"
#include "TileSourceUrlImpl.h"

#include <sstream>

namespace MapWindow {
void addMark(const GeoCoords& coords, const std::string& name);
std::string getFsPathFromMapView(int mapView);
void startTileProviderConnectivityTest();
void updateMarkStyle();

constexpr const char* INI_MAP_WINDOW_MAP_VIEW = "map_window_map_view";

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
} // namespace MapWindow

void MapWindow::init() {
    constexpr const int GCS_TILE_REQUEST_LIMIT = 25;

    mapPlot = std::make_shared<RichMapPlot>();
    storage = std::make_shared<MarkStorage>();
    urlConnectionTest = std::make_shared<TileSourceUrlConnTest>();

    std::string mapTileDir = getFsPathFromMapView(MAP_VIEW);
    std::string satelliteTileDir = getFsPathFromMapView(SATELLITE_VIEW);
    mapTileGrabber = std::make_shared<TileGrabber>(std::make_shared<TileSourceUrlOsm>(GCS_TILE_REQUEST_LIMIT, MAP_PRELOAD),
                                                   std::make_shared<TileSaverSubDir>(mapTileDir));
    satelliteTileGrabber = std::make_shared<TileGrabber>(std::make_shared<TileSourceUrlArc>(GCS_TILE_REQUEST_LIMIT, MAP_PRELOAD),
                                                         std::make_shared<TileSaverSubDir>(satelliteTileDir));

    addMark({46.14665264871996, -70.66861153239353}, "Tapis Venture");

    updateMarkStyle();
}

void MapWindow::loadState(const mINI::INIStructure& ini) {
    mapPlot->loadState(ini);
    if (ini.has(IniConfig::GCS_SECTION)) {
        if (ini.get(IniConfig::GCS_SECTION).has(INI_MAP_WINDOW_MAP_VIEW)) {
            mapView = std::stoi(ini.get(IniConfig::GCS_SECTION).get(INI_MAP_WINDOW_MAP_VIEW));
        }
    }
}

void MapWindow::saveState(mINI::INIStructure& ini) {
    mapPlot->saveState(ini);
    ini[IniConfig::GCS_SECTION].set(INI_MAP_WINDOW_MAP_VIEW, std::to_string(mapView));
}

void MapWindow::render() {
    constexpr const size_t GCS_MAP_MAX_TILES_DOWNLOAD = 100'000;

    if (ImGui::CollapsingHeader("Download Tiles")) {
        ImGui::Indent(20.0f);

        size_t tilesSelectedTotal =
          2 * countTiles(mapPlot->minLat(), mapPlot->maxLat(), mapPlot->minLon(), mapPlot->maxLon(), downloadMinZ, downloadMaxZ);
        bool maxTilesDownloadExceeded = tilesSelectedTotal > GCS_MAP_MAX_TILES_DOWNLOAD;

        // Latitude and longitude
        ImGui::Text("Min Lat %.6f, Min Lon %.6f", mapPlot->minLat(), mapPlot->minLon());
        ImGui::Text("Max Lat %.6f, Max Lon %.6f", mapPlot->maxLat(), mapPlot->maxLon());

        // Input to set which zoom levels we want to download the tiles from + tile counter
        ImGui::SetNextItemWidth(200);
        ImGui::InputInt("Min Z", &downloadMinZ);
        downloadMinZ = std::clamp(downloadMinZ, 0, MAX_ZOOM);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputInt("Max Z", &downloadMaxZ);
        downloadMaxZ = std::clamp(downloadMaxZ, 0, MAX_ZOOM);

        // Tile count and zoom level
        if (maxTilesDownloadExceeded) {
            ImGui::Text("Tiles Count:");
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::Text(">%zu (Too many tiles, can't download)", GCS_MAP_MAX_TILES_DOWNLOAD);
            ImGui::PopStyleColor();
        } else {
            ImGui::Text("Tiles Count: %zu", tilesSelectedTotal);
        }
        ImGui::SameLine();
        ImGui::Text("Current Zoom Level: %d", mapPlot->zoom());

        // Download button
        ImGui::BeginDisabled(sourceIsFs || maxTilesDownloadExceeded || !mapTileGrabber->done() || !satelliteTileGrabber->done());
        if (ImGui::Button("Download")) {
            downloadTileTotal = tilesSelectedTotal;
            mapTileGrabber->grab(mapPlot->minLat(), mapPlot->maxLat(), mapPlot->minLon(), mapPlot->maxLon(), downloadMinZ, downloadMaxZ);
            satelliteTileGrabber->grab(mapPlot->minLat(), mapPlot->maxLat(), mapPlot->minLon(), mapPlot->maxLon(), downloadMinZ, downloadMaxZ);
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            if (sourceIsFs) {
                ImGui::SetTooltip(
                  "Map tiles cannot be downloaded because the tile provider cannot be accessed (likely because of no Internet access).");
            } else if (maxTilesDownloadExceeded) {
                ImGui::SetTooltip("Downloads exceeding %zu tiles are not permitted.", GCS_MAP_MAX_TILES_DOWNLOAD);
            } else {
                ImGui::SetTooltip("Download map tiles that are currently on screen so they can be accessed without having to fetch them online.");
            }
        }
        ImGui::EndDisabled();
        ImGui::SameLine();

        // Stop download button
        ImGui::BeginDisabled((mapTileGrabber->done() || mapTileGrabber->isStopping())
                             && (satelliteTileGrabber->done() || satelliteTileGrabber->isStopping()));
        if (ImGui::Button("Stop")) {
            mapTileGrabber->stop();
            satelliteTileGrabber->stop();
        }
        ImGui::EndDisabled();

        // Download progress bar
        downloadTileCount = mapTileGrabber->tileCounter() + satelliteTileGrabber->tileCounter();
        downloadProgress = float(downloadTileCount) / float(downloadTileTotal);
        downloadProgress = std::isnan(downloadProgress) ? 0.f : downloadProgress;
        ImGui::SameLine();
        ImGui::Text("%zu/%zu", downloadTileCount, downloadTileTotal);
        ImGui::SameLine();
        ImGui::ProgressBar(downloadProgress);

        ImGui::Unindent(20.0f);
    }

    ImGui::RadioButton("Map", &mapView, MAP_VIEW);
    ImGui::SameLine();
    ImGui::RadioButton("Satellite", &mapView, SATELLITE_VIEW);

    bool hasSwitchedSource = mapView != prevMapView;
    auto now = std::chrono::steady_clock::now();
    bool autoSourceSwitchDelayElapsed = now - lastAutoSourceSwitchTime > autoSourceSwitchDelay;

    // Check if we can fetch tiles from online provider
    // If we can't, fetch the downloaded tiles locally
    if (mapPlot->failedToFetchTiles() && autoSourceSwitchDelayElapsed && !sourceIsFs) {
        sourceIsFs = true;
        lastAutoSourceSwitchTime = now;
        hasSwitchedSource = true;
        GCS_APP_LOG_DEBUG("MapWindow: Lost connection to tile provider.");
    } else if (sourceIsFs) {
        startTileProviderConnectivityTest();
        if (urlConnectionTest->successful() && autoSourceSwitchDelayElapsed) {
            sourceIsFs = false;
            lastAutoSourceSwitchTime = now;
            hasSwitchedSource = true;
            GCS_APP_LOG_DEBUG("MapWindow: Regained connection to tile provider.");
        }
    }

    if (hasSwitchedSource) {
        updateMarkStyle();
        if (sourceIsFs) {
            mapPlot->setTileLoader(std::make_shared<TileLoaderFsMap>(getFsPathFromMapView(mapView)));
        } else {
            if (mapView == MAP_VIEW) {
                mapPlot->setTileLoader(std::make_shared<TileLoaderOsmMap>());
            } else if (mapView == SATELLITE_VIEW) {
                mapPlot->setTileLoader(std::make_shared<TileLoaderArcMap>());
            }
        }
        prevMapView = mapView;
    }

    mapPlot->paint();
}

void MapWindow::addMark(const GeoCoords& coords, const std::string& name) {
    storage->addMark(coords, name);
    mapPlot->addItem(std::reinterpret_pointer_cast<IRichItem>(storage->markItems().back().ptr));
}

std::string MapWindow::getFsPathFromMapView(int mapView) {
    std::string mapViewFolder;
    if (mapView == MAP_VIEW) {
        mapViewFolder = "map";
    } else if (mapView == SATELLITE_VIEW) {
        mapViewFolder = "satellite";
    }
    return (std::filesystem::current_path() / "tiles" / mapViewFolder).string();
}

void MapWindow::startTileProviderConnectivityTest() {
    std::ostringstream urlmaker;
    if (mapView == MAP_VIEW) {
        urlmaker << TileSourceUrlOsm::makeSourceUrl(0, 0, 0);
    } else if (mapView == SATELLITE_VIEW) {
        urlmaker << TileSourceUrlArcImagery::makeSourceUrl(0, 0, 0);
    }
    urlConnectionTest->startConnectivityTest(urlmaker.str());
}

void MapWindow::updateMarkStyle() {
    MarkItem::Style style;

    style.markerWeight = 1.f;

    if (mapView == MAP_VIEW) {
        style.markerFill = ImVec4(0.f, 0.f, 0.f, 1.f);
        style.markerOutline = ImVec4(1.f, 1.f, 1.f, 1.f);
    } else if (mapView == SATELLITE_VIEW) {
        style.markerFill = ImVec4(1.f, 1.f, 1.f, 1.f);
        style.markerOutline = ImVec4(0.f, 0.f, 0.f, 1.f);
    }

    for (auto& item : storage->markItems()) {
        item.ptr->setStyle(style);
    }
}
