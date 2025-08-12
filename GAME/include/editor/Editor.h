// include/editor/Editor.h
#pragma once
#include "ecs.h"
#include "components.h"
#include <imgui.h>  


class Editor {
public:
    Editor(ECS& ecs_ref, bool& paused_ref);
    void renderGUI();

private:
    ECS& ecs;
    bool& paused;
    Entity selectedEntity = -1; 

    void drawEntityList();
    void drawInspector();
    void drawControls();
};