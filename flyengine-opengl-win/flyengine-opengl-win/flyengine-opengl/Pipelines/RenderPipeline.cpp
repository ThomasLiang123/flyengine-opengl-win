//
//  RenderPipeline.cpp
//  flyengine
//
//  Created by Thomas Liang on 10/24/23.
//

#include "RenderPipeline.h"

# define PI           3.14159265358979323846

RenderPipeline::~RenderPipeline() {
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow * RenderPipeline::init () {
    if (!glfwInit()) {
        return nullptr;
    }
    
    // Create window with graphics context
    window_width = 1280;
    window_height = 720;
    window = glfwCreateWindow(window_width, window_height, "flyengine", nullptr, nullptr);
    if (window == nullptr)
        return nullptr;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup IO
    io.WantCaptureKeyboard = true;

    // Setup style
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
    
    return window;
}

void RenderPipeline::SetScheme(Scheme *sch) {
    scheme = sch;
}

void RenderPipeline::SetSchemeController(SchemeController *sctr) {
    scheme_controller = sctr;
}

void RenderPipeline::SetBuffers(std::vector<vector_float3> *v, std::vector<Face> *f) {
    vertex_buffer = v;
    face_buffer = f;
}

void RenderPipeline::RenderScene() {
    // camera and perspective
    Camera *c = scheme->GetCamera();
    if (c == nullptr) return;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(c->FOV.y*180/PI, (GLfloat) window_width / (GLfloat) window_height, 1.f, 100.f);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    gluLookAt(c->pos.x, c->pos.z, c->pos.y,    // Eye-position
              c->pos.x + c->vector.x, c->pos.z + c->vector.z, c->pos.y + c->vector.y,   // View-point
              c->up_vector.x, c->up_vector.z, c->up_vector.y);   // Up-vector
    
    Scene *s = scheme->GetScene();
    int fid = 0;
    for (int i = 0; i < s->NumModels(); i++) {
        Model *m = s->GetModel(i);
        glBegin(GL_TRIANGLES);
        glLoadIdentity();
        for (int j = 0; j < m->NumFaces(); j++) {
            if (fid >= face_buffer->size()) {
                break;
            }
            
            Face *f = &(face_buffer->at(fid));
            glColor3f(f->color.x, f->color.y, f->color.z);
            glVertex3f(vertex_buffer->at(f->vertices[0]).x, vertex_buffer->at(f->vertices[0]).z, vertex_buffer->at(f->vertices[0]).y);
            glVertex3f(vertex_buffer->at(f->vertices[1]).x, vertex_buffer->at(f->vertices[1]).z, vertex_buffer->at(f->vertices[1]).y);
            glVertex3f(vertex_buffer->at(f->vertices[2]).x, vertex_buffer->at(f->vertices[2]).z, vertex_buffer->at(f->vertices[2]).y);
            
            fid++;
        }
        glEnd();
    }
}

void RenderPipeline::Render() {
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    scheme_controller->BuildUI();
    
    scheme = scheme_controller->GetScheme();
    scheme->BuildUI();
    
    ImGui::Render();
    glfwGetFramebufferSize(window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);
    
    ImVec4 clear_color = ImVec4(0.6f, 0.6f, 0.6f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    RenderScene();
    
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}
