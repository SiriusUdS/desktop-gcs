#include "SavableStateWidget.h"

#include "ThemedColors.h"

#include <imgui.h>

SavableStateWidget::SavableStateWidget(State& state, std::string label) : state(state), label(label) {
    readButtonLabel = "Read##read_" + state.id;
    saveButtonLabel = "Save##save_" + state.id;
    cancelButtonLabel = "Cancel##cancel_" + state.id;
}

void SavableStateWidget::renderAsRow(float savableValue) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", label.c_str());

    ImGui::TableSetColumnIndex(1);
    ImGui::BeginDisabled(saved);
    if (ImGui::Button(readButtonLabel.c_str())) {
        readValue = savableValue;
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(2);
    ImGui::BeginDisabled(saved);
    if (ImGui::Button(saveButtonLabel.c_str())) {
        state.value = readValue;
        saved = true;
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(3);
    ImGui::BeginDisabled(!saved);
    if (ImGui::Button(cancelButtonLabel.c_str())) {
        saved = false;
        readValue = 0;
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(4);
    ImGui::Text("ADC Value: %.0f", readValue);

    ImGui::TableSetColumnIndex(5);
    if (saved) {
        ImVec4 color = ThemedColors::Text::green.resolve();
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("SAVED");
    } else {
        ImVec4 color = ThemedColors::Text::red.resolve();
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("UNSAVED");
    }
    ImGui::PopStyleColor();
}
