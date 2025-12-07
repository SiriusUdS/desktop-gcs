#ifndef STATEMACHINERENDERER_H
#define STATEMACHINERENDERER_H

#include <imgui.h>
#include <vector>

class StateMachineRenderer {
public:
    struct StateRect {
        ImVec2 position;
        ImVec2 size;
        bool active{};
        const char* label{"UNKNOWN"};
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

public:
    void addStateRect(StateRect rect);
    void addArrow(Arrow arrow);
    void render(ImVec2 size, bool drawDebugRegions = false);

    static Arrow createArrow(const StateRect& rect1,
                             AnchorEdge anchorRect1,
                             const StateRect& rect2,
                             AnchorEdge anchorRect2,
                             ArrowPathType pathType = ArrowPathType::HORIZONTAL,
                             float routeOffset = 0.0f);
    static ImVec2 getAnchorPointPosition(const StateRect& rect, AnchorEdge anchorEdge);

private:
    void computeAvailableSpace(const ImVec2& size);
    void computeMiddlePoint();
    void computeBoundaries();
    void computeOffsetPosition();
    void computeSizeScale();

    void drawStateRect(const StateRect& rect);
    void drawArrow(const Arrow& arrow);

    ImVec2 getWindowPosFromStateMachinePos(const ImVec2& stateMachinePos);

    void drawDebugRegion(const ImVec2& size);
    void drawDebugPadding(const ImVec2& size);
    void drawDebugMiddlePoint(const ImVec2& size);

private:
    static const float PADDING;

    std::vector<StateRect> rects;
    std::vector<Arrow> arrows;

    ImVec2 availableSpace;
    ImVec2 middlePoint;
    ImVec2 topLeftBoundary;
    ImVec2 bottomRightBoundary;
    ImVec2 offsetPosition;
    float sizeScale{1};
};

#endif // STATEMACHINERENDERER_H
