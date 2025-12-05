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
        bool arrowhead{};
        ArrowheadDir arrowheadDirection{ArrowheadDir::UP};
    };

    enum class ArrowType { HORIZONTAL, VERTICAL };

    enum class AnchorPointDir { TOP, RIGHT, BOTTOM, LEFT };

public:
    void addStateRect(StateRect rect);
    void addArrow(Arrow arrow);
    void render(ImVec2 size, bool drawDebugRegions = false);

    static Arrow createArrow(const StateRect& rect1, AnchorPointDir anchorRect1, const StateRect& rect2, AnchorPointDir anchorRect2, ArrowType type);
    static ImVec2 getAnchorPointPosition(const StateRect& rect, AnchorPointDir dir);

private:
    void computeAvailableSpace(const ImVec2& size);
    void computeMiddlePoint();
    void computeBoundaries();
    void computeOffsetPosition();
    void computeSizeScale();

    void drawStateRect(const StateRect& rect);
    void drawArrow(const Arrow& arrow);

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
