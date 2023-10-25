//
//  ComputePipeline.h
//  flyengine
//
//  Created by Thomas Liang on 10/24/23.
//

#ifndef ComputePipeline_h
#define ComputePipeline_h

#include <stdio.h>
#include <vector>
#include <string>

#include "Utils/Vec.h"
using namespace Vec;

#include "../Schemes/Scheme.h"
#include "RenderPipeline.h"

class ComputePipeline {
private:
    // buffers for scene compute
    std::vector<vector_float3> vertex_buffer;
    std::vector<Face> face_buffer;
    
    // scheme and scheme variables
    Scheme *scheme;
    unsigned int num_scene_vertices = 0;
    unsigned int num_scene_faces = 0;
    
    void CalculateVertices();
    void CalculateFaceLighting();
public:
    void init();
    void SetScheme(Scheme *sch);
    
    void SetEmptyBuffers();
    void ResetStaticBuffers();
    void ResetDynamicBuffers();
    
    void Compute();
    void SendDataToRenderer(RenderPipeline *renderer);
};

#endif /* ComputePipeline_h */
