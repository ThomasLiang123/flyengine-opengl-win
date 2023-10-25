//
//  GenerateLighting.cpp
//  dragonfly
//
//  Created by Thomas Liang on 10/24/23.
//

#include "Normals.h"
# define PI_2           3.14159265358979323846/2

int DragonflyUtils::FaceIntercept(Model *m, vector_float3 start, vector_float3 vector, int avoid) {
    int fid = -1;
    float minz = -1;
    
    for (int i = 0; i < m->NumFaces(); i++) {
        if (i != avoid) {
            Face *f = m->GetFace(i);
            
            Vertex v1 = m->GetVertex(f->vertices[0]);
            Vertex v2 = m->GetVertex(f->vertices[1]);
            Vertex v3 = m->GetVertex(f->vertices[2]);
            vector_float4 plane = PlaneEquation(v1, v2, v3);
            float t = LineAndPlane(start, vector, plane);
            
            if (t > 0 && t < 10000) {
                vector_float3 p = vector_make_float3(start.x + t * vector.x, start.y + t * vector.y, start.z + t * vector.z);
                
                float subtrianglearea = TriangleArea(v1, v2, p) + TriangleArea(v1, v3, p) + TriangleArea(v2, v3, p);
                //            std::cout<<i<<std::endl;
                //            std::cout<<"("<<v1.x<<", "<<v1.y<<", "<<v1.z<<")"<<std::endl;
                //            std::cout<<"("<<v2.x<<", "<<v2.y<<", "<<v2.z<<")"<<std::endl;
                //            std::cout<<"("<<v3.x<<", "<<v3.y<<", "<<v3.z<<")"<<std::endl;
                //            std::cout<<"("<<p.x<<", "<<p.y<<", "<<p.z<<")"<<std::endl;
                //            std::cout<<"("<<plane.x<<", "<<plane.y<<", "<<plane.z<<", "<<plane.w<<")"<<std::endl;
                //            std::cout<<t<<std::endl;
                //            std::cout<<subtrianglearea<<" "<<TriangleArea(v1, v2, v3)<<std::endl;
                if (subtrianglearea < TriangleArea(v1, v2, v3) * 1.05) {
                    float z = dist3to3(start, p);
                    
                    if (minz == -1 || z < minz) {
                        minz = z;
                        fid = i;
                    }
                }
            }
        }
    }
    
    return fid;
}

void DragonflyUtils::FindNormals(Model *m) {
    vector_float3 origin = vector_make_float3(0, 0, 0);
    
    float radius = 0;
    for (int i = 0; i < m->NumVertices(); i++) {
        Vertex v = m->GetVertex(i);
        origin.x += v.x;
        origin.y += v.y;
        origin.z += v.z;
    }
    
    origin.x /= m->NumVertices();
    origin.y /= m->NumVertices();
    origin.z /= m->NumVertices();
    
    for (int i = 0; i < m->NumVertices(); i++) {
        float dist = dist3to3(m->GetVertex(i), origin);
        if (dist > radius) {
            radius = dist;
        }
    }
    radius *= 1.5;
//    std::cout<<radius<<std::endl;
    
    std::vector<bool> visited_faces(m->NumFaces());
    for (int i = 0; i < m->NumFaces(); i++) {
        visited_faces[i] = false;
    }
    
    srand (time(NULL));
    Face *currface = m->GetFace(0);
    vector_float3 center = TriAvg(m->GetVertex(currface->vertices[0]), m->GetVertex(currface->vertices[1]), m->GetVertex(currface->vertices[2]));
    vector_float3 vector = GetNormal(m->GetVertex(currface->vertices[0]), m->GetVertex(currface->vertices[1]), m->GetVertex(currface->vertices[2]));
    
    center.x += vector.x;
    center.y += vector.y;
    center.z += vector.z;
    
    visited_faces[0] = true;
    int num_visited = 1;
    
    for (int i = 0; i < 100000; i++) {
        int nextfid = FaceIntercept(m, center, vector, /*nextfid*/-1 /*huh??*/);
//        std::cout<<std::endl;
        
        if (nextfid != -1) {
            currface = m->GetFace(nextfid);
            center = TriAvg(m->GetVertex(currface->vertices[0]), m->GetVertex(currface->vertices[1]), m->GetVertex(currface->vertices[2]));
            vector_float3 normal = GetNormal(m->GetVertex(currface->vertices[0]), m->GetVertex(currface->vertices[1]), m->GetVertex(currface->vertices[2]));
            
            //std::cout<<acos2(normal, vector)<<std::endl;
            if (abs(acos2(normal, vector)) < PI_2) {
                currface->normal_reversed = true;
                normal.x *= -1;
                normal.y *= -1;
                normal.z *= -1;
            } else {
                currface->normal_reversed = false;
            }
            
            vector_float3 vecend = vector_make_float3(center.x + normal.x, center.y + normal.y, center.z + normal.z);
            
            // rotate within 45 degrees randomly
            vector_float3 newend = RotateAround(vecend, center, vector_make_float3((float(rand() % 1600) / 1000) - 0.8, (float(rand() % 1600) / 1000) - 0.8, (float(rand() % 1600) / 1000) - 0.8));
            
            vector.x = newend.x - center.x;
            vector.y = newend.y - center.y;
            vector.z = newend.z - center.z;
            
            center.x += vector.x;
            center.y += vector.y;
            center.z += vector.z;
            
            if (!visited_faces[nextfid]) {
                visited_faces[nextfid] = true;
                num_visited++;
            }
        } else {
            vector_float3 distpoly = DistancePolynomial(center, vector, origin);
            distpoly.z -= pow(radius, 2);
            float t = QuadraticEquation(distpoly);
            
            center.x += t * vector.x;
            center.y += t * vector.y;
            center.z += t * vector.z;
            
            vector_float3 edgevec = vector_make_float3(origin.x-center.x, origin.y-center.y, origin.z-center.z);
            float mag = dist3to3(edgevec, vector_make_float3(0, 0, 0));
            edgevec.x /= mag;
            edgevec.y /= mag;
            edgevec.z /= mag;
            
            vector_float3 vecend = vector_make_float3(center.x + edgevec.x, center.y + edgevec.y, center.z + edgevec.z);
            
            // rotate within 45 degrees randomly
            vector_float3 newend = RotateAround(vecend, center, vector_make_float3((float(rand() % 800) / 1000) - 0.4, (float(rand() % 800) / 1000) - 0.4, (float(rand() % 800) / 1000) - 0.4));
            
            vector.x = newend.x - center.x;
            vector.y = newend.y - center.y;
            vector.z = newend.z - center.z;
            
            center.x += vector.x;
            center.y += vector.y;
            center.z += vector.z;
        }
    }
    
    float percent_visited = float(num_visited) / m->NumFaces();
    std::cout<<"percent faces visited: "<<percent_visited*100<<std::endl;
}

void DragonflyUtils::ReverseNormals(Model *m) {
    for (int i = 0; i < m->NumFaces(); i++) {
        m->GetFace(i)->normal_reversed = !m->GetFace(i)->normal_reversed;
    }
}
