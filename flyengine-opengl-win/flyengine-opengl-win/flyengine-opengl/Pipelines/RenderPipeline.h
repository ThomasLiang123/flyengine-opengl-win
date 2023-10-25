//
//  RenderPipeline.h
//  flyengine
//
//  Created by Thomas Liang on 10/24/23.
//

#ifndef RenderPipeline_h
#define RenderPipeline_h

#include <stdio.h>
#include <vector>
#include <string>

#include "Utils/Vec.h"
using namespace Vec;

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "../Schemes/Scheme.h"
#include "../Schemes/SchemeController.h"

class RenderPipeline {
private:
    GLFWwindow *window;
    
    int window_width;
    int window_height;
    
    // buffers for scene render
    std::vector<vector_float3> *vertex_buffer;
    std::vector<Face> *face_buffer;
    
    Scheme *scheme;
    SchemeController *scheme_controller;
    
    void RenderScene();
public:
    ~RenderPipeline();
    
    GLFWwindow *init();
    void SetScheme(Scheme *sch);
    void SetSchemeController(SchemeController *sctr);
    void SetBuffers(std::vector<vector_float3> *v, std::vector<Face> *f);
    
    void Render();
};

#endif /* RenderPipeline_h */
