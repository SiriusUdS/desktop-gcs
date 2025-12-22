#include "StateMachineRenderer.h"

#include "FontConfig.h"
#include "ThemedColors.h"

StateMachineRenderer::StateMachineRenderer() {
}

StateMachineRenderer::StateMachineRenderer(Params params) : params{params} {
}

void StateMachineRenderer::addStateRect(StateRect rect) {
    rects.push_back(rect);
}

void StateMachineRenderer::addArrow(Arrow arrow) {
    arrows.push_back(arrow);
}

void StateMachineRenderer::addLabel(Label label) {
    labels.push_back(label);
}

void StateMachineRenderer::render(ImVec2 size, bool drawDebugRegions) {
    updateBoundaries();
    size = handleSizeOptions(size);
    availableSpace = computeAvailableSpace(size);
    middlePoint = computeMiddlePoint();
    offsetPosition = computeOffsetPosition();
    sizeScale = computeSizeScale();

    for (const auto& rect : rects) {
        drawStateRect(rect);
    }

    for (const auto& arrow : arrows) {
        drawArrow(arrow);
    }

    if (params.showArrowLabels) {
        for (const auto& arrow : arrows) {
            drawArrowLabel(arrow);
        }
    }

    for (const auto& label : labels) {
        drawLabel(label);
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
                                                              AnchorEdge anchorEdge1,
                                                              const StateRect& rect2,
                                                              AnchorEdge anchorEdge2,
                                                              ArrowPathType pathType,
                                                              float routeOffset) {
    const ImVec2 p1 = getStateRectAnchorPointPosition(rect1, anchorEdge1);
    const ImVec2 p2 = getStateRectAnchorPointPosition(rect2, anchorEdge2);
    return createArrowFromAnchorPoints(p1, anchorEdge1, p2, pathType, routeOffset);
}

StateMachineRenderer::Arrow StateMachineRenderer::createArrow(const StateRect& rect,
                                                              AnchorEdge rectAnchorEdge,
                                                              const Label& label,
                                                              AnchorEdge labelAnchorEdge,
                                                              ArrowPathType pathType,
                                                              float routeOffset) {
    const ImVec2 p1 = getStateRectAnchorPointPosition(rect, rectAnchorEdge);
    const ImVec2 p2 = getLabelAnchorPointPosition(label, labelAnchorEdge);
    return createArrowFromAnchorPoints(p1, rectAnchorEdge, p2, pathType, routeOffset);
}

void StateMachineRenderer::updateBoundaries() {
    if (rects.empty()) {
        topLeftBoundary = {0, 0};
        bottomRightBoundary = {0, 0};
        return;
    }

    ImVec2 topLeft = {FLT_MAX, FLT_MAX};
    ImVec2 bottomRight = {-FLT_MAX, -FLT_MAX};

    // Compute boundaries from rects
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

    // Compute boundaries from arrows
    for (const auto& arrow : arrows) {
        for (const auto& point : arrow.points) {
            if (point.x < topLeft.x) {
                topLeft.x = point.x;
            }
            if (point.x > bottomRight.x) {
                bottomRight.x = point.x;
            }
            if (point.y < topLeft.y) {
                topLeft.y = point.y;
            }
            if (point.y > bottomRight.y) {
                bottomRight.y = point.y;
            }
        }
    }

    topLeftBoundary = topLeft;
    bottomRightBoundary = bottomRight;
}

ImVec2 StateMachineRenderer::handleSizeOptions(ImVec2 size) {
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    if (size.x == -1.0f) {
        size.x = contentRegionAvail.x;
    } else if (size.x == 0.0f) {
        size.x = bottomRightBoundary.x - topLeftBoundary.x;
    }

    if (size.y == -1.0f) {
        size.y = contentRegionAvail.y;
    } else if (size.y == 0.0f) {
        size.y = bottomRightBoundary.y - topLeftBoundary.y;
    }

    return size;
}

ImVec2 StateMachineRenderer::computeAvailableSpace(const ImVec2& size) {
    return {std::max(size.x - params.windowPadding * 2.0f, 0.0f), std::max(size.y - params.windowPadding * 2.0f, 0.0f)};
}

ImVec2 StateMachineRenderer::computeMiddlePoint() {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    return {cursorPos.x + params.windowPadding + availableSpace.x / 2, cursorPos.y + params.windowPadding + availableSpace.y / 2};
}

ImVec2 StateMachineRenderer::computeOffsetPosition() {
    ImVec2 stateMachineMiddlePoint = {(topLeftBoundary.x + bottomRightBoundary.x) / 2, (topLeftBoundary.y + bottomRightBoundary.y) / 2};
    return {-stateMachineMiddlePoint.x, -stateMachineMiddlePoint.y};
}

float StateMachineRenderer::computeSizeScale() {
    if (rects.empty()) {
        return 1;
    }

    ImVec2 stateMachineSize = {bottomRightBoundary.x - topLeftBoundary.x, bottomRightBoundary.y - topLeftBoundary.y};
    ImVec2 excessSize = {std::max(0.0f, stateMachineSize.x - availableSpace.x), std::max(0.0f, stateMachineSize.y - availableSpace.y)};
    ImVec2 excessRatio = {excessSize.x == 0 ? 0 : excessSize.x / stateMachineSize.x, excessSize.y == 0 ? 0 : excessSize.y / stateMachineSize.y};

    return 1 - std::max(excessRatio.x, excessRatio.y);
}

void StateMachineRenderer::drawStateRect(const StateRect& rect) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Rect position
    ImVec2 rectWindowPos = getWindowPosFromStateMachinePos(rect.position);
    ImVec2 rectMin = {rectWindowPos.x - rect.size.x * sizeScale / 2, rectWindowPos.y - rect.size.y * sizeScale / 2};
    ImVec2 rectMax = {rectMin.x + rect.size.x * sizeScale, rectMin.y + rect.size.y * sizeScale};

    // Text position
    float textFontSize = params.stateRectLabelFontSize * sizeScale;
    ImVec2 textSize = FontConfig::mainFont->CalcTextSizeA(textFontSize, FLT_MAX, -1.0f, rect.label);
    ImVec2 textPosition = {rectWindowPos.x - textSize.x / 2, rectWindowPos.y - textSize.y / 2};

    // Style
    ImColor color = rect.active ? ThemedColors::StateMachine::activeState.resolve() : ThemedColors::StateMachine::inactiveState.resolve();
    ImColor borderColor = ImGui::GetColorU32(ImGuiCol_Text);
    ImColor textColor = ImGui::GetColorU32(ImGuiCol_Text);

    drawList->AddRectFilled(rectMin, rectMax, color, params.stateRectRounding);
    drawList->AddRect(rectMin, rectMax, borderColor, params.stateRectRounding, 0, params.stateRectBorderThickness);
    drawList->AddText(FontConfig::mainFont, textFontSize, textPosition, textColor, rect.label);
}

void StateMachineRenderer::drawArrow(const Arrow& arrow) {
    if (arrow.points.size() < 2) {
        return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Style
    ImColor arrowColor = ThemedColors::StateMachine::arrow.resolve();

    // Arrow segments
    for (size_t i = 0; i < arrow.points.size() - 1; i++) {
        const ImVec2& p1 = arrow.points[i];
        const ImVec2& p2 = arrow.points[i + 1];
        ImVec2 segmentStart = getWindowPosFromStateMachinePos(p1);
        ImVec2 segmentEnd = getWindowPosFromStateMachinePos(p2);

        drawList->AddLine(segmentStart, segmentEnd, arrowColor, params.arrowThickness);
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

        drawList->AddLine(segmentEnd, arrowPoint1, arrowColor, params.arrowThickness);
        drawList->AddLine(segmentEnd, arrowPoint2, arrowColor, params.arrowThickness);
    }
}

void StateMachineRenderer::drawArrowLabel(const Arrow& arrow) {
    if (arrow.label == "") {
        return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 labelPosition;
    size_t pointsAmount = arrow.points.size();

    if (pointsAmount % 2 == 0) {
        const ImVec2& midPoint1 = arrow.points[(pointsAmount / 2) - 1];
        const ImVec2& midPoint2 = arrow.points[pointsAmount / 2];
        labelPosition = {(midPoint1.x + midPoint2.x) / 2, (midPoint1.y + midPoint2.y) / 2};
    } else {
        labelPosition = arrow.points[pointsAmount / 2];
    }
    labelPosition = {labelPosition.x + arrow.labelOffset.x, labelPosition.y + arrow.labelOffset.y};

    const float textFontSize = params.arrowLabelFontSize * sizeScale;
    const ImVec2 textSize = FontConfig::mainFont->CalcTextSizeA(textFontSize, FLT_MAX, -1.0f, arrow.label);
    ImVec2 labelWindowPosition = getWindowPosFromStateMachinePos(labelPosition);
    labelWindowPosition = {labelWindowPosition.x - textSize.x / 2, labelWindowPosition.y - textSize.y / 2};

    float bgRectPadding = 1.0f;
    const ImVec2 rectMin = {labelWindowPosition.x - bgRectPadding, labelWindowPosition.y - bgRectPadding};
    const ImVec2 rectMax = {labelWindowPosition.x + textSize.x + bgRectPadding, labelWindowPosition.y + textSize.y + bgRectPadding};

    const ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
    const ImColor bg_u32 = ImGui::GetColorU32(bg);

    const ImColor textColor = ImGui::GetColorU32(ImGuiCol_Text);

    drawList->AddRectFilled(rectMin, rectMax, bg_u32);
    drawList->AddText(FontConfig::mainFont, textFontSize, labelWindowPosition, textColor, arrow.label);
}

void StateMachineRenderer::drawLabel(const Label& label) {
    if (label.text == "") {
        return;
    }

    ImVec2 labelWindowPosition = getWindowPosFromStateMachinePos(label.position);
    float textFontSize = params.labelFontSize * sizeScale;
    ImVec2 textSize = FontConfig::mainFont->CalcTextSizeA(textFontSize, FLT_MAX, -1.0f, label.text);
    labelWindowPosition = {labelWindowPosition.x - textSize.x / 2, labelWindowPosition.y - textSize.y / 2};

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImColor textColor = ImGui::GetColorU32(ImGuiCol_Text);
    drawList->AddText(FontConfig::mainFont, textFontSize, labelWindowPosition, textColor, label.text);
}

ImVec2 StateMachineRenderer::getWindowPosFromStateMachinePos(const ImVec2& stateMachinePos) {
    return {(stateMachinePos.x + offsetPosition.x) * sizeScale + middlePoint.x, (stateMachinePos.y + offsetPosition.y) * sizeScale + middlePoint.y};
}

ImVec2 StateMachineRenderer::getStateRectAnchorPointPosition(const StateRect& rect, AnchorEdge anchorEdge) {
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

ImVec2 StateMachineRenderer::getLabelAnchorPointPosition(const Label& label, AnchorEdge anchorEdge) {
    ImVec2 textSize = ImGui::CalcTextSize(label.text);
    ImVec2 labelSize = {textSize.x + params.arrowLabelPadding * 2, textSize.y + params.arrowLabelPadding * 2};

    switch (anchorEdge.side) {
    case AnchorEdgeSide::TOP:
        return {label.position.x + labelSize.x * (anchorEdge.position - 0.5f), label.position.y - labelSize.y / 2};
    case AnchorEdgeSide::RIGHT:
        return {label.position.x + labelSize.x / 2, label.position.y + labelSize.y * (anchorEdge.position - 0.5f)};
    case AnchorEdgeSide::BOTTOM:
        return {label.position.x - labelSize.x * (anchorEdge.position - 0.5f), label.position.y + labelSize.y / 2};
    case AnchorEdgeSide::LEFT:
        return {label.position.x - labelSize.x / 2, label.position.y - labelSize.y * (anchorEdge.position - 0.5f)};
    default:
        return {0, 0};
    }
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
    ImVec2 rectMin = {cursorPos.x + params.windowPadding, cursorPos.y + params.windowPadding};
    ImVec2 rectMax = {cursorPos.x + size.x - params.windowPadding, cursorPos.y + size.y - params.windowPadding};
    ImColor rectColor = IM_COL32(0, 0, 255, 255);

    drawList->AddRect(rectMin, rectMax, rectColor);
}

void StateMachineRenderer::drawDebugMiddlePoint(const ImVec2& size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float radius = 10.0f;
    ImColor color = IM_COL32(0, 0, 255, 255);
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 midPoint = computeMiddlePoint();

    drawList->AddCircleFilled(midPoint, radius, color);
}

StateMachineRenderer::Arrow StateMachineRenderer::createArrowFromAnchorPoints(const ImVec2& p1,
                                                                              AnchorEdge anchorEdge1,
                                                                              const ImVec2& p2,
                                                                              ArrowPathType pathType,
                                                                              float routeOffset) {
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
        if (anchorEdge1.side == AnchorEdgeSide::TOP || anchorEdge1.side == AnchorEdgeSide::BOTTOM) {
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
