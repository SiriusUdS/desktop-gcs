#include "Layout.h"

#include "UIWindow.h"

const char* Layout::MAIN_DOCKSPACE = "MainDockSpace";

HelloImGui::DockingParams Layout::createDockingParams() {
    return {.dockingSplits = createDockingSplits(), .dockableWindows = createDockableWindows(), .layoutName = getName()};
}

std::vector<HelloImGui::DockableWindow> Layout::createDockableWindows() {
    std::vector<DockedWindow> dockedWindows = getDockedWindows();
    std::vector<HelloImGui::DockableWindow> dockableWindows;

    for (const auto& dockedWindow : dockedWindows) {
        dockableWindows.emplace_back(dockedWindow.window->getName(), dockedWindow.dockspace, [dockedWindow]() { dockedWindow.window->render(); });
    }

    return dockableWindows;
}
