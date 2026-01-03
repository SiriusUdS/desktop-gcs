#pragma once

#include <hello_imgui/hello_imgui.h>
#include <vector>

class UIWindow;

class Layout {
public:
    virtual ~Layout() = default;

    HelloImGui::DockingParams createDockingParams();

protected:
    struct DockedWindow {
        std::shared_ptr<UIWindow> window;
        const char* dockspace;
    };

    std::vector<HelloImGui::DockableWindow> createDockableWindows();

    virtual const char* getName() = 0;
    virtual std::vector<HelloImGui::DockingSplit> createDockingSplits() = 0;
    virtual std::vector<DockedWindow> getDockedWindows() = 0;

    static const char* MAIN_DOCKSPACE;
};
