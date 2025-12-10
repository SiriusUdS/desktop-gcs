#pragma once

#include "Coords.h"
#include "IRichItem.h"

#include <cmath>
#include <string>
#include <vector>

/**
 * @class MarkItem
 * @brief A visual marker on a map plot.
 */
class MarkItem : public IRichItem {
public:
    /**
     * @struct MarkItem::Style
     * @brief The style of a mark item.
     */
    struct Style {
        // using ImPlotMarker = int;
        bool textEnabled{true};                       ///< Is text shown.
        bool markerEnabled{true};                     ///< Is marker shown.
        bool radiusEnabled{true};                     ///< Is radius shown.
        ImPlotMarker markerType{ImPlotMarker_Circle}; ///< The marker type.
        float markerSize{10.f};                       ///< The size of the marker.
        float markerWeight{};                         ///< The weight of the marker.
        ImVec4 markerFill{1.f, 1.f, 1.f, 1.f};        ///< Fill color of the marker.
        ImVec4 markerOutline{};                       ///< Outline color of the marker.
        float radiusWeight{2.f};                      ///< The weight of the radius.
    };

    MarkItem();
    MarkItem(const GeoCoords& coords, const std::string& text);
    virtual ~MarkItem() override;

    virtual bool inBounds(float minLat, float maxLat, float minLon, float maxLon) const override;

    /**
     * @brief Set the visibility of the mark item.
     */
    virtual void setEnabled(bool enabled) override {
        _enabled = enabled;
    }

    /**
     * @brief Returns true if the mark item is enabled.
     */
    virtual bool enabled() const override {
        return _enabled;
    }

    virtual void paint() override;

    /**
     * @brief Set the latitude/longitude coordinates of the item.
     */
    inline void setCoords(const GeoCoords& coords) {
        _geoCoords = coords;
        _osmCoords = _geoCoords.toOsmCoords();
        if (_r > 0.0) {
            updateRadiusPoints();
            updateRadiusBounds();
        }
    }

    /**
     * @brief Get the latitude/longitude coordinates of the item.
     */
    inline const GeoCoords& geoCoords() const {
        return _geoCoords;
    }

    /**
     * @brief Get the Osm coordinates of the item.
     */
    inline const OsmCoords& osmCoords() const {
        return _osmCoords;
    }

    /**
     * @brief Set the mark item radius.
     */
    inline void setRadius(float r) {
        _r = r;
        updateRadiusPoints();
        updateRadiusBounds();
    }

    /**
     * @brief Get the mark item radius.
     */
    inline float radius() const {
        return _r;
    }

    /**
     * @brief Set the mark item text.
     */
    inline void setText(const std::string& text) {
        _text = text;
    }

    /**
     * @brief Get the mark item text.
     */
    inline const std::string& text() const {
        return _text;
    }

    /**
     * @brief Set the mark item style.
     */
    inline void setStyle(const Style& style) {
        _style = style;
    }

    /**
     * @brief Get the mark item style.
     */
    inline const Style& style() const {
        return _style;
    }

    /**
     * @brief Get the mark item style.
     */
    inline Style& style() {
        return _style;
    }

private:
    void updateRadiusPoints();
    void updateRadiusBounds();

private:
    bool _enabled{true};
    GeoCoords _geoCoords{};
    OsmCoords _osmCoords{};
    std::string _text;
    double _latTL{}, _latTR{}, _latBL{}, _latBR{};
    double _lonTL{}, _lonTR{}, _lonBL{}, _lonBR{};
    float _r{};
    std::vector<float> _rx, _ry;
    float _dphi{1.0};
    Style _style;
};
