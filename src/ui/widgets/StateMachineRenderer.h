#pragma once

#include <imgui.h>
#include <vector>

class StateMachineRenderer {
public:
    struct Params {
        bool showArrowLabels{true};
        float arrowLabelFontSize{23.0f};
        float arrowLabelPadding{15.0f};
        float arrowThickness{2.0f};
        float labelFontSize{32.0f};
        float stateRectBorderThickness{2.0f};
        float stateRectRounding{10.0f};
        float stateRectLabelFontSize{32.0f};
        float windowPadding{10.0f};
    };

    struct StateRect {
        ImVec2 position;
        ImVec2 size;
        bool& active;
        const char* label{""};
    };

    enum class ArrowheadDir { UP, RIGHT, DOWN, LEFT };

    struct Arrow {
        std::vector<ImVec2> points;
        bool arrowhead{true};
        ArrowheadDir arrowheadDirection{ArrowheadDir::UP};
        const char* label{""};
        ImVec2 labelOffset{0, 0};
    };

    enum class ArrowPathType { HORIZONTAL, VERTICAL, ORTHOGONAL };

    enum class AnchorEdgeSide { TOP, RIGHT, BOTTOM, LEFT };

    struct AnchorEdge {
        AnchorEdgeSide side;
        float position{0.5}; /// Position along the edge, value goes from 0 to 1
    };

    struct Label {
        ImVec2 position;
        const char* text{""};
    };

public:
    StateMachineRenderer();
    StateMachineRenderer(Params params);

    void addStateRect(StateRect rect);
    void addArrow(Arrow arrow);
    void addLabel(Label label);
    void render(ImVec2 size = {-1.0f, 0.0f}, bool drawDebugRegions = false);

    // State rect to state rect
    Arrow createArrow(const StateRect& rect1,
                      AnchorEdge anchorEdge1,
                      const StateRect& rect2,
                      AnchorEdge anchorEdge2,
                      ArrowPathType pathType = ArrowPathType::HORIZONTAL,
                      float routeOffset = 0.0f);

    // State rect to label
    Arrow createArrow(const StateRect& rect,
                      AnchorEdge rectAnchorEdge,
                      const Label& label,
                      AnchorEdge labelAnchorEdge,
                      ArrowPathType pathType = ArrowPathType::HORIZONTAL,
                      float routeOffset = 0.0f);

    Params params;

private:
    void updateBoundaries();
    ImVec2 handleSizeOptions(ImVec2 size);
    ImVec2 computeAvailableSpace(const ImVec2& size);
    ImVec2 computeMiddlePoint();
    ImVec2 computeOffsetPosition();
    float computeSizeScale();

    void drawStateRect(const StateRect& rect);
    void drawArrow(const Arrow& arrow);
    void drawArrowLabel(const Arrow& arrow);
    void drawLabel(const Label& label);

    ImVec2 getWindowPosFromStateMachinePos(const ImVec2& stateMachinePos);

    ImVec2 getStateRectAnchorPointPosition(const StateRect& rect, AnchorEdge anchorEdge);
    ImVec2 getLabelAnchorPointPosition(const Label& label, AnchorEdge anchorEdge);

    void drawDebugRegion(const ImVec2& size);
    void drawDebugPadding(const ImVec2& size);
    void drawDebugMiddlePoint(const ImVec2& size);

    Arrow createArrowFromAnchorPoints(const ImVec2& p1,
                                      AnchorEdge anchorEdge1,
                                      const ImVec2& p2,
                                      ArrowPathType pathType = ArrowPathType::HORIZONTAL,
                                      float routeOffset = 0.0f);

private:
    std::vector<StateRect> rects;
    std::vector<Arrow> arrows;
    std::vector<Label> labels;

    ImVec2 availableSpace;
    ImVec2 middlePoint;
    ImVec2 topLeftBoundary;
    ImVec2 bottomRightBoundary;
    ImVec2 offsetPosition;
    float sizeScale{1};
};
