//
//  ComputePipeline.cpp
//  flyengine
//
//  Created by Thomas Liang on 10/24/23.
//

#include "ComputePipeline.h"
#include <iostream>

void ComputePipeline::init() {
    
}

void ComputePipeline::SetScheme(Scheme *sch) {
    scheme = sch;
    
    num_scene_vertices = scheme->NumSceneVertices();
    num_scene_faces = scheme->NumSceneFaces();
}

void ComputePipeline::SetEmptyBuffers() {
    num_scene_vertices = scheme->NumSceneVertices();
    num_scene_faces = scheme->NumSceneFaces();
    
    int vid = 0;
    int fid = 0;
    Scene *s = scheme->GetScene();
    for (int i = 0; i < s->NumModels(); i++) {
        Model *m = s->GetModel(i);
        m->SetVertexStart(vid);
        m->SetFaceStart(fid);
        vid += m->NumVertices();
        fid += m->NumFaces();
    }
}

void ComputePipeline::ResetStaticBuffers() {
    num_scene_vertices = scheme->NumSceneVertices();
    num_scene_faces = scheme->NumSceneFaces();
    
    vertex_buffer = std::vector<vector_float3>(num_scene_vertices);
    face_buffer = std::vector<Face>(num_scene_faces);
    
    Scene *s = scheme->GetScene();
    int model_idx = 0;
    for (int i = 0; i < num_scene_faces; i++) {
        if (i >= face_buffer.size()) {
            return;
        }
        
        Model *m = s->GetModel(model_idx);
        if (i - m->FaceStart() >= m->NumFaces()) {
            model_idx++;
            m = s->GetModel(model_idx);
        }
        Face *orig_f = m->GetFace(i - m->FaceStart());
        Face f = *orig_f;
        f.vertices[0] += m->VertexStart();
        f.vertices[1] += m->VertexStart();
        f.vertices[2] += m->VertexStart();
        face_buffer[i] = f;
    }
}

void ComputePipeline::ResetDynamicBuffers() {
    
}

void ComputePipeline::CalculateVertices() {
    int model_idx = 0;
    Scene *s = scheme->GetScene();
    for (int i = 0; i < num_scene_vertices; i++) {
        if (i >= vertex_buffer.size()) {
            return;
        }
        
        Model *m = s->GetModel(model_idx);
        ModelUniforms *mu = s->GetModelUniforms(model_idx);
        if (i - m->VertexStart() >= m->NumVertices()) {
            model_idx++;
            if (model_idx >= s->NumModels()) break;
            m = s->GetModel(model_idx);
            mu = s->GetModelUniforms(model_idx);
        }
        
        vector_float3 rel_vertex = m->GetVertex(i - m->VertexStart());
        vector_float3 abs_vertex = DragonflyUtils::TranslatePointToStandard(&mu->b, rel_vertex);
        vertex_buffer[i] = abs_vertex;
    }
}

void ComputePipeline::CalculateFaceLighting() {
    
}

void ComputePipeline::Compute() {
    CalculateVertices();
    CalculateFaceLighting();
}


void ComputePipeline::SendDataToRenderer(RenderPipeline *renderer) {
    renderer->SetBuffers(&vertex_buffer, &face_buffer);
}
