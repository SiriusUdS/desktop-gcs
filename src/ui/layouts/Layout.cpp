#include "Layout.h"

#include "UIWindow.h"

const char* Layout::MAIN_DOCKSPACE = "MainDockSpace";

HelloImGui::DockingParams Layout::createDockingParams() {
    return {.dockingSplits = createDockingSplits(), .dockableWindows = createDockableWindows(), .layoutName = getName()};
}

std::vector<HelloImGui::DockableWindow> Layout::createDockableWindows() {
    std::vector<DockedWindow> temp = getDockedWindows();
    std::vector<HelloImGui::DockableWindow> dockableWindows;

    for (const auto& test : temp) {
        dockableWindows.emplace_back(test.window->getName(), test.dockspace, [test]() { test.window->render(); });
    }

    return dockableWindows;
}
