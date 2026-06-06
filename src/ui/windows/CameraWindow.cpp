#include "CameraWindow.h"

#include <imgui.h>
#include <spdlog/common.h>

CameraWindow::CameraWindow() {
    
}

const char* CameraWindow::getName() const {
    return "Camera Window";
}

void CameraWindow::renderImpl() {
    const auto& renderData = CameraManager::get().getRenderData();
    if (renderData.empty()) {
        ImGui::Text("No active camera feed");
    }
    
    for (size_t i = 0; i < renderData.size(); i++) {
        const auto& cam = renderData[i];
        
        ImGui::BeginGroup();
        ImGui::Text("Camera %d", cam.id);
        
        if (cam.textureId != 0) {
            ImGui::Image((ImTextureID)cam.textureId, ImVec2(400, 225));
        }else {
            ImGui::Dummy(ImVec2(400, 225));
            ImGui::TextDisabled("Waiting for stream...");
        }
        ImGui::EndGroup();
        
        if ((i + 1)%2 != 0 && i < renderData.size()-1) {
            ImGui::SameLine();
        }
    }
}