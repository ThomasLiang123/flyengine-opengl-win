//
//  main.cpp
//  flyengine-opengl
//
//  Created by Thomas Liang on 10/24/23.
//

#include <iostream>
#include "Engine.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    
    Engine *engine = new Engine();
    
    if (engine->init() == 0) {
        engine->run();
    }
    
    delete engine;
    
    return 0;
}
