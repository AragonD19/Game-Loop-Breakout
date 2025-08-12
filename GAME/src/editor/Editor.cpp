// src/editor/Editor.cpp
#include "editor/Editor.h"
#include <imgui.h>
#include <string>

Editor::Editor(ECS& ecs_ref, bool& paused_ref) : ecs(ecs_ref), paused(paused_ref) {}

void Editor::renderGUI() {
    
    drawControls();
    
    drawEntityList();
    
    if (selectedEntity != -1) {
        drawInspector();
    }
}


void Editor::drawControls() {
    ImGui::Begin("Game Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Button(paused ? "Resume" : "Pause")) {
        paused = !paused;
    }
    ImGui::End();
}

void Editor::drawEntityList() {
    ImGui::Begin("Entities");

    // Lista todas las entidades activas (itera sobre todos los maps conocidos)
    ImGui::Text("Active Entities:");
    if (ImGui::TreeNode("Paddles")) {
        for (auto& [e, _] : ecs.getComponentMap<PaddleControlled>()) {
            std::string label = "Entity " + std::to_string(e);
            if (ImGui::Selectable(label.c_str(), selectedEntity == e)) {
                selectedEntity = e;
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Balls")) {
        for (auto& [e, _] : ecs.getComponentMap<Ball>()) {
            std::string label = "Entity " + std::to_string(e);
            if (ImGui::Selectable(label.c_str(), selectedEntity == e)) {
                selectedEntity = e;
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Blocks")) {
        for (auto& [e, _] : ecs.getComponentMap<Block>()) {
            std::string label = "Entity " + std::to_string(e);
            if (ImGui::Selectable(label.c_str(), selectedEntity == e)) {
                selectedEntity = e;
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void Editor::drawInspector() {
    ImGui::Begin("Inspector");

    ImGui::Text("Selected Entity: %zu", selectedEntity);

    // Muestra y edita componentes
    if (auto* pos = ecs.getComponent<Position>(selectedEntity)) {
        ImGui::Text("Position:");
        ImGui::InputFloat("X", &pos->x);
        ImGui::InputFloat("Y", &pos->y);
    }
    if (auto* vel = ecs.getComponent<Velocity>(selectedEntity)) {
        ImGui::Text("Velocity:");
        ImGui::InputFloat("VX", &vel->vx);
        ImGui::InputFloat("VY", &vel->vy);
    }
    if (auto* size = ecs.getComponent<Size>(selectedEntity)) {
        ImGui::Text("Size:");
        ImGui::InputFloat("Width", &size->w);
        ImGui::InputFloat("Height", &size->h);
    }
    

    ImGui::End();
}