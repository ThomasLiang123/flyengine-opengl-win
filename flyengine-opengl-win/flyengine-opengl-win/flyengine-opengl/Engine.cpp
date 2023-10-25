//
//  Engine.cpp
//  flyengine-test
//
//  Created by Thomas Liang on 10/24/23.
//

#include "Engine.h"
# define PI_2           3.14159265358979323846/2

std::queue<int> Engine::presses;
std::queue<int> Engine::releases;
int Engine::mouse_x = 0;
int Engine::mouse_y = 0;
int Engine::mouse_dx = 0;
int Engine::mouse_dy = 0;
std::queue<int> Engine::mouse_presses;
std::queue<int> Engine::mouse_releases;

Engine::Engine() {
}

Engine::~Engine() {
    delete compute_pipeline;
    delete render_pipeline;
    
    delete camera;
    delete scheme;
    delete scheme_controller;
}

int Engine::init() {
    camera = new Camera();
    camera->pos = {-2, 0, 0};
    camera->vector = {1, 0, 0};
    camera->up_vector = {0, 0, 1};
    camera->FOV = {PI_2, PI_2};
    
    scheme = new PlayScheme();
    scheme->SetCamera(camera);
    
    scheme_controller = new SchemeController(scheme);
    
    compute_pipeline = new ComputePipeline();
    compute_pipeline->init();
    compute_pipeline->SetScheme(scheme);
    
    render_pipeline = new RenderPipeline();
    window = render_pipeline->init();
    if (window == nullptr) {
        return -1;
    }
    render_pipeline->SetScheme(scheme);
    render_pipeline->SetSchemeController(scheme_controller);
    
    compute_pipeline->ResetStaticBuffers();
    compute_pipeline->SetEmptyBuffers();
    
    glfwSetKeyCallback(window, HandleKeyboardEvents);
    glfwSetMouseButtonCallback(window, HandleMouseButton);
    glfwSetCursorPosCallback(window, HandleCursorMovement);
    
    return 0;
}

void Engine::run() {
    // Main loop
    bool done = false;
    while (!done && !glfwWindowShouldClose(window)) {
        // inputs
        glfwPollEvents();
        
        while (!presses.empty()) {
            scheme->HandleKeyPresses(presses.front(), true);
            presses.pop();
        }
        
        while (!releases.empty()) {
            scheme->HandleKeyPresses(releases.front(), false);
            releases.pop();
        }
        
        while (!mouse_presses.empty()) {
            if (mouse_presses.front() == 0) {
                scheme->HandleMouseDown(vector_make_float2(mouse_x, mouse_y), true);
            } else if (mouse_presses.front() == 1) {
                scheme->HandleMouseDown(vector_make_float2(mouse_x, mouse_y), false);
            }
            mouse_presses.pop();
        }
        
        while (!mouse_releases.empty()) {
            if (mouse_releases.front() == 0) {
                scheme->HandleMouseUp(vector_make_float2(mouse_x, mouse_y), true);
            } else if (mouse_releases.front() == 1) {
                scheme->HandleMouseUp(vector_make_float2(mouse_x, mouse_y), false);
            }
            mouse_releases.pop();
        }
        
        scheme->HandleMouseMovement(mouse_x, mouse_y, mouse_dx, mouse_dy);
        mouse_dx = 0;
        mouse_dy = 0;
        
        // render
        if (scheme->ShouldResetStaticBuffers()) {
            compute_pipeline->ResetStaticBuffers();
            scheme->SetResetStaticBuffers(false);
        }
        if (scheme->ShouldResetEmptyBuffers()) {
            compute_pipeline->SetEmptyBuffers();
            scheme->SetResetEmptyBuffers(false);
        }
        compute_pipeline->ResetDynamicBuffers();

        compute_pipeline->Compute();
        compute_pipeline->SendDataToRenderer(render_pipeline);
        
        //RenderUI();
        render_pipeline->Render();
        scheme = scheme_controller->GetScheme();
        compute_pipeline->SetScheme(scheme);
        
        scheme->Update();
    }
}

void Engine::HandleKeyboardEvents(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        presses.push(key);
    } else if (action == GLFW_RELEASE) {
        releases.push(key);
    }
}

void Engine::HandleMouseButton(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse_presses.push(0);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        mouse_presses.push(1);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse_releases.push(0);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        mouse_releases.push(1);
    }
}

void Engine::HandleCursorMovement(GLFWwindow* window, double xpos, double ypos) {
    mouse_dx = xpos - mouse_x;
    mouse_dy = ypos - mouse_y;
    mouse_x = xpos;
    mouse_y = ypos;
}
