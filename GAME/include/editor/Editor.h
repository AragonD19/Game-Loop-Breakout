// include/editor/Editor.h
#pragma once
#include "../ecs.h"
#include "../components.h"
#include <imgui.h>

class Scene;  

class Editor {
public:
    Editor(bool& paused_ref);  
    void renderGUI(Scene* currentScene);  

private:
    bool& paused;
    Entity selectedEntity = -1;

    void drawEntityList(ECS& ecs);
    void drawInspector(ECS& ecs);
    void drawControls();
};