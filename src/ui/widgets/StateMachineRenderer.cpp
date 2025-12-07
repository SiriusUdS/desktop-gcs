#include "StateMachineRenderer.h"

#include "ThemedColors.h"

const float StateMachineRenderer::PADDING = 40.0f;

void StateMachineRenderer::addStateRect(StateRect rect) {
    rects.push_back(rect);
}

void StateMachineRenderer::addArrow(Arrow arrow) {
    arrows.push_back(arrow);
}

void StateMachineRenderer::render(ImVec2 size, bool drawDebugRegions) {
    // TODO: Make these functions return values instead of having extra attributes
    computeAvailableSpace(size);
    computeMiddlePoint();
    computeBoundaries();
    computeOffsetPosition();
    computeSizeScale();

    for (const auto& rect : rects) {
        drawStateRect(rect);
    }

    for (const auto& arrow : arrows) {
        drawArrow(arrow);
    }

    if (drawDebugRegions) {
        drawDebugRegion(size);
        drawDebugPadding(size);
        drawDebugMiddlePoint(size);
    }

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos({cursorPos.x, cursorPos.y + size.y});
}

StateMachineRenderer::Arrow StateMachineRenderer::createArrow(const StateRect& rect1,
                                                              AnchorEdge anchorRect1,
                                                              const StateRect& rect2,
                                                              AnchorEdge anchorRect2,
                                                              ArrowPathType pathType,
                                                              float routeOffset) {
    const ImVec2 p1 = getAnchorPointPosition(rect1, anchorRect1);
    const ImVec2 p2 = getAnchorPointPosition(rect2, anchorRect2);
    std::vector<ImVec2> points;

    // Create arrow path
    points.push_back(p1);
    if (p1.x == p2.x || p1.y == p2.y) {
        // Do nothing, straight line
    } else if (pathType == ArrowPathType::HORIZONTAL) {
        float midSegmentY = (p1.y + p2.y) / 2.0f + routeOffset;
        points.push_back({p1.x, midSegmentY});
        points.push_back({p2.x, midSegmentY});
    } else if (pathType == ArrowPathType::VERTICAL) {
        float midSegmentX = (p1.x + p2.x) / 2.0f + routeOffset;
        points.push_back({midSegmentX, p1.y});
        points.push_back({midSegmentX, p2.y});
    } else if (pathType == ArrowPathType::ORTHOGONAL) {
        if (anchorRect1.side == AnchorEdgeSide::TOP || anchorRect1.side == AnchorEdgeSide::BOTTOM) {
            points.push_back({p1.x, p2.y});
        } else {
            points.push_back({p2.x, p1.y});
        }
    }
    points.push_back(p2);

    // Determine arrowhead direction
    ArrowheadDir dir = ArrowheadDir::UP;
    const ImVec2& ps = points.at(points.size() - 2); // Second to last point
    if (p2.y > ps.y) {
        dir = ArrowheadDir::DOWN;
    } else if (p2.x > ps.x) {
        dir = ArrowheadDir::RIGHT;
    } else if (p2.x < ps.x) {
        dir = ArrowheadDir::LEFT;
    }

    return {.points{points}, .arrowheadDirection{dir}};
}

ImVec2 StateMachineRenderer::getAnchorPointPosition(const StateRect& rect, AnchorEdge anchorEdge) {
    switch (anchorEdge.side) {
    case AnchorEdgeSide::TOP:
        return {rect.position.x + rect.size.x * (anchorEdge.position - 0.5f), rect.position.y - rect.size.y / 2};
    case AnchorEdgeSide::RIGHT:
        return {rect.position.x + rect.size.x / 2, rect.position.y + rect.size.y * (anchorEdge.position - 0.5f)};
    case AnchorEdgeSide::BOTTOM:
        return {rect.position.x - rect.size.x * (anchorEdge.position - 0.5f), rect.position.y + rect.size.y / 2};
    case AnchorEdgeSide::LEFT:
        return {rect.position.x - rect.size.x / 2, rect.position.y - rect.size.y * (anchorEdge.position - 0.5f)};
    default:
        return {0, 0};
    }
}

void StateMachineRenderer::computeAvailableSpace(const ImVec2& size) {
    availableSpace = {size.x - PADDING * 2, size.y - PADDING * 2};
}

void StateMachineRenderer::computeMiddlePoint() {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    middlePoint = {cursorPos.x + PADDING + availableSpace.x / 2, cursorPos.y + PADDING + availableSpace.y / 2};
}

void StateMachineRenderer::computeBoundaries() {
    if (rects.empty()) {
        topLeftBoundary = {0, 0};
        bottomRightBoundary = {0, 0};
        return;
    }

    ImVec2 topLeft = {FLT_MAX, FLT_MAX};
    ImVec2 bottomRight = {-FLT_MAX, -FLT_MAX};

    for (const auto& rect : rects) {
        if (rect.position.x - rect.size.x / 2 < topLeft.x) {
            topLeft.x = rect.position.x - rect.size.x / 2;
        }

        if (rect.position.x + rect.size.x / 2 > bottomRight.x) {
            bottomRight.x = rect.position.x + rect.size.x / 2;
        }

        if (rect.position.y - rect.size.y / 2 < topLeft.y) {
            topLeft.y = rect.position.y - rect.size.y / 2;
        }

        if (rect.position.y + rect.size.y / 2 > bottomRight.y) {
            bottomRight.y = rect.position.y + rect.size.y / 2;
        }
    }

    topLeftBoundary = topLeft;
    bottomRightBoundary = bottomRight;
}

void StateMachineRenderer::computeOffsetPosition() {
    ImVec2 stateMachineMiddlePoint = {(topLeftBoundary.x + bottomRightBoundary.x) / 2, (topLeftBoundary.y + bottomRightBoundary.y) / 2};
    offsetPosition = {-stateMachineMiddlePoint.x, -stateMachineMiddlePoint.y};
}

void StateMachineRenderer::computeSizeScale() {
    if (rects.empty()) {
        sizeScale = 1;
        return;
    }

    ImVec2 stateMachineSize = {bottomRightBoundary.x - topLeftBoundary.x, bottomRightBoundary.y - topLeftBoundary.y};
    ImVec2 excessSize = {std::max(0.0f, stateMachineSize.x - availableSpace.x), std::max(0.0f, stateMachineSize.y - availableSpace.y)};
    ImVec2 excessRatio = {excessSize.x == 0 ? 0 : excessSize.x / stateMachineSize.x, excessSize.y == 0 ? 0 : excessSize.y / stateMachineSize.y};

    sizeScale = 1 - std::max(excessRatio.x, excessRatio.y);
}

void StateMachineRenderer::drawStateRect(const StateRect& rect) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Rect position
    ImVec2 rectWindowPos = getWindowPosFromStateMachinePos(rect.position);
    ImVec2 rectMin = {rectWindowPos.x - rect.size.x * sizeScale / 2, rectWindowPos.y - rect.size.y * sizeScale / 2};
    ImVec2 rectMax = {rectMin.x + rect.size.x * sizeScale, rectMin.y + rect.size.y * sizeScale};

    // Text position
    ImVec2 textSize = ImGui::CalcTextSize(rect.label);
    ImVec2 textPosition = {rectWindowPos.x - textSize.x / 2, rectWindowPos.y - textSize.y / 2};

    // Style
    ImColor color = rect.active ? IM_COL32(170, 255, 170, 255) : IM_COL32(255, 170, 170, 255);
    ImColor borderColor = IM_COL32(0, 0, 0, 255);
    ImColor textColor = IM_COL32(0, 0, 0, 255);
    float rounding = 10.0f;
    float borderThickness = 2.0f;

    drawList->AddRectFilled(rectMin, rectMax, color, rounding);
    drawList->AddRect(rectMin, rectMax, borderColor, rounding, 0, borderThickness);
    drawList->AddText(textPosition, textColor, rect.label);
}

void StateMachineRenderer::drawArrow(const Arrow& arrow) {
    if (arrow.points.size() < 2) {
        return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Style
    ImColor arrowColor = ThemedColors::STATE_MACHINE_ARROW.resolve();
    float arrowThickness = 2.0f;

    // Arrow segments
    for (size_t i = 0; i < arrow.points.size() - 1; i++) {
        const ImVec2& p1 = arrow.points[i];
        const ImVec2& p2 = arrow.points[i + 1];
        ImVec2 segmentStart = getWindowPosFromStateMachinePos(p1);
        ImVec2 segmentEnd = getWindowPosFromStateMachinePos(p2);

        drawList->AddLine(segmentStart, segmentEnd, arrowColor, arrowThickness);
    }

    // Arrowhead
    if (arrow.arrowhead) {
        const ImVec2& lastPoint = arrow.points.back();
        ImVec2 segmentEnd = getWindowPosFromStateMachinePos(lastPoint);
        ImVec2 arrowPoint1, arrowPoint2;
        float arrowSize = 20.0f * sizeScale;
        float arrowheadBaseOffset = -8.0f * sizeScale;

        switch (arrow.arrowheadDirection) {
        case ArrowheadDir::UP:
            arrowPoint1 = {segmentEnd.x - arrowSize - arrowheadBaseOffset, segmentEnd.y + arrowSize};
            arrowPoint2 = {segmentEnd.x + arrowSize + arrowheadBaseOffset, segmentEnd.y + arrowSize};
            break;
        case ArrowheadDir::RIGHT:
            arrowPoint1 = {segmentEnd.x - arrowSize, segmentEnd.y - arrowSize - arrowheadBaseOffset};
            arrowPoint2 = {segmentEnd.x - arrowSize, segmentEnd.y + arrowSize + arrowheadBaseOffset};
            break;
        case ArrowheadDir::DOWN:
            arrowPoint1 = {segmentEnd.x - arrowSize - arrowheadBaseOffset, segmentEnd.y - arrowSize};
            arrowPoint2 = {segmentEnd.x + arrowSize + arrowheadBaseOffset, segmentEnd.y - arrowSize};
            break;
        case ArrowheadDir::LEFT:
            arrowPoint1 = {segmentEnd.x + arrowSize, segmentEnd.y - arrowSize - arrowheadBaseOffset};
            arrowPoint2 = {segmentEnd.x + arrowSize, segmentEnd.y + arrowSize + arrowheadBaseOffset};
            break;
        }

        drawList->AddLine(segmentEnd, arrowPoint1, arrowColor, arrowThickness);
        drawList->AddLine(segmentEnd, arrowPoint2, arrowColor, arrowThickness);
    }

    // Label
    if (arrow.label != "") {
        ImVec2 labelPosition;
        size_t points_amount = arrow.points.size();

        if (points_amount % 2 == 0) {
            const ImVec2& midPoint1 = arrow.points[(points_amount / 2) - 1];
            const ImVec2& midPoint2 = arrow.points[points_amount / 2];
            labelPosition = {(midPoint1.x + midPoint2.x) / 2, (midPoint1.y + midPoint2.y) / 2};
        } else {
            labelPosition = arrow.points[points_amount / 2];
        }

        ImVec2 labelWindowPosition = getWindowPosFromStateMachinePos(labelPosition);
        ImVec2 textSize = ImGui::CalcTextSize(arrow.label);
        labelWindowPosition = {labelWindowPosition.x - textSize.x / 2, labelWindowPosition.y - textSize.y / 2};

        float bgRectPadding = 2.0f;
        ImVec2 rectMin = {labelWindowPosition.x - bgRectPadding, labelWindowPosition.y - bgRectPadding};
        ImVec2 rectMax = {labelWindowPosition.x + textSize.x + bgRectPadding, labelWindowPosition.y + textSize.y + bgRectPadding};

        ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImU32 bg_u32 = ImGui::GetColorU32(bg);

        drawList->AddRectFilled(rectMin, rectMax, bg_u32);
        drawList->AddText(labelWindowPosition, arrowColor, arrow.label);
    }
}

ImVec2 StateMachineRenderer::getWindowPosFromStateMachinePos(const ImVec2& stateMachinePos) {
    return {(stateMachinePos.x + offsetPosition.x) * sizeScale + middlePoint.x, (stateMachinePos.y + offsetPosition.y) * sizeScale + middlePoint.y};
}

void StateMachineRenderer::drawDebugRegion(const ImVec2& size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 rectMax = {cursorPos.x + size.x, cursorPos.y + size.y};
    ImColor rectColor = IM_COL32(255, 0, 0, 255);

    drawList->AddRect(cursorPos, rectMax, rectColor);
}

void StateMachineRenderer::drawDebugPadding(const ImVec2& size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 rectMin = {cursorPos.x + PADDING, cursorPos.y + PADDING};
    ImVec2 rectMax = {cursorPos.x + size.x - PADDING, cursorPos.y + size.y - PADDING};
    ImColor rectColor = IM_COL32(0, 0, 255, 255);

    drawList->AddRect(rectMin, rectMax, rectColor);
}

void StateMachineRenderer::drawDebugMiddlePoint(const ImVec2& size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float radius = 10.0f;
    ImColor color = IM_COL32(0, 0, 255, 255);
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 midPoint = {cursorPos.x + size.x / 2, cursorPos.y + size.y / 2}; // TODO: Use computeMiddlePoint in the future

    drawList->AddCircleFilled(midPoint, radius, color);
}
