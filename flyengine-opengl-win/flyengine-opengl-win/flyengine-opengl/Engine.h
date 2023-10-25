//
//  Engine.h
//  flyengine-test
//
//  Created by Thomas Liang on 10/24/23.
//

#ifndef Engine_h
#define Engine_h

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <queue>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#include "Utils/Vec.h"
using namespace Vec;

#include "Camera.h"

#include "Schemes/SchemeController.h"

#include "Schemes/Scheme.h"
#include "Schemes/PlayScheme.h"
#include "Schemes/ViewScheme.h"

#include "Pipelines/ComputePipeline.h"
#include "Pipelines/RenderPipeline.h"

class Engine {
private:
    std::string project_path = "/";
    
    GLFWwindow *window;

    int scene_window_start_x = 0;
    int scene_window_start_y = 0;
    int window_width = 1280;
    int window_height = 720;

    bool show_main_window = true;

    float fps = 0;
    
    ComputePipeline *compute_pipeline;
    RenderPipeline *render_pipeline;

    // scheme and scene
    Camera *camera;
    SchemeController *scheme_controller;
    Scheme *scheme;
    
    static void HandleKeyboardEvents(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void HandleMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void HandleCursorMovement(GLFWwindow* window, double xpos, double ypos);
public:
    // inputs
    static std::queue<int> presses;
    static std::queue<int> releases;
    static int mouse_x;
    static int mouse_y;
    static int mouse_dx;
    static int mouse_dy;
    // 0 left, 1 right
    static std::queue<int> mouse_presses;
    static std::queue<int> mouse_releases;
    
    
    Engine();
    ~Engine();
    int init();
    void run();
};

#endif /* Engine_h */
