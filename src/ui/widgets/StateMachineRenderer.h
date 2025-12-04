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

    enum class ArrowheadDirection { UP, RIGHT, DOWN, LEFT };

    struct Arrow {
        std::vector<ImVec2> points;
        bool arrowhead{};
        ArrowheadDirection arrowheadDirection{ArrowheadDirection::UP};
    };

public:
    void addStateRect(StateRect rect);
    void addArrow(Arrow arrow);
    void render(ImVec2 size, bool drawDebugRegions = false);

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
    float sizeScale;
};

#endif // STATEMACHINERENDERER_H
