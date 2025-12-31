#include "Application.h"

#include <hello_imgui/hello_imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main(int, char*[]) {
    Application::init();

    HelloImGui::RunnerParams runnerParams;

    runnerParams.appWindowParams.windowTitle = "Sirius GCS";
    runnerParams.appWindowParams.restorePreviousGeometry = true;

    runnerParams.appWindowParams.borderless = false;
    runnerParams.appWindowParams.borderlessMovable = true;
    runnerParams.appWindowParams.borderlessResizable = true;
    runnerParams.appWindowParams.borderlessClosable = true;

    runnerParams.dockingParams.dockingSplits = Application::createBaseDockingSplits();
    runnerParams.dockingParams.dockableWindows = Application::createDockableWindows();

    runnerParams.callbacks.LoadAdditionalFonts = Application::loadFonts;
    runnerParams.callbacks.PreNewFrame = Application::preNewFrame;
    runnerParams.callbacks.ShowMenus = Application::showMenus;
    runnerParams.callbacks.BeforeExit = Application::shutdown;

    runnerParams.imGuiWindowParams.menuAppTitle = "Sirius GCS";
    runnerParams.imGuiWindowParams.showMenuBar = true;
    runnerParams.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;

    runnerParams.fpsIdling.rememberEnableIdling = true;

    runnerParams.iniFilename = "hello_imgui.ini";

    HelloImGui::Run(runnerParams);
}
