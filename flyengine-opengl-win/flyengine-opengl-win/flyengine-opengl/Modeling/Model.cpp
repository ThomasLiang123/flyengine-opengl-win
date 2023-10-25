//
//  Model.cpp
//  dragonfly
//
//  Created by Thomas Liang on 10/24/23.
//

#include "Model.h"
#include <cstddef>
#include <iostream>
//#include <filesystem>
//namespace fs = std::filesystem;

Animation::Animation(Model *model) {
    model_ = model;
    
    for (int i = 0; i < model_->NumNodes(); i++) {
        node_animations.push_back(new std::vector<NodeKeyFrame *>());
    }
}

Animation::~Animation() {
    for (int i = 0; i < model_->NumNodes(); i++) {
        std::vector<NodeKeyFrame *> *kfs = node_animations[i];
        
        for (int j = 0; j < kfs->size(); j++) {
            free(kfs->at(j));
        }
        free(kfs);
    }
}

float Animation::GetLength() {
    return length;
}

std::pair<int, int> Animation::FindFrameIdx(uint32_t nid, float time) {
    std::vector<NodeKeyFrame *> *frames = node_animations[nid];

    if (frames->empty()) {
        return std::make_pair(0, 1);
    }

    int start = 0;
    int end = frames->size()-1;
    int mid = (start + end) / 2;

    if (frames->at(end)->time < time) {
        return std::make_pair(end, end+1);
    } else if (frames->at(end)->time == time) {
        return std::make_pair(end, end);
    }

    if (frames->at(start)->time > time) {
        return std::make_pair(start-1, start);
    } else if (frames->at(start)->time == time) {
        return std::make_pair(start, start);
    }

    while (end - start > 1) {
        if (frames->at(mid)->time < time) {
            start = mid;
        } else if (frames->at(mid)->time > time) {
            end = mid;
        } else {
            start = mid;
            end = mid;
        }

        mid = (start + end) / 2;
    }

    return std::make_pair(start, end);
}


void Animation::AddNode() {
    node_animations.push_back(new std::vector<NodeKeyFrame *>());
}

void Animation::SetKeyFrame(uint32_t nid, float time, Basis basis) {
    if (time > length) {
        length = time;
    }
    
    std::vector<NodeKeyFrame *> *frames = node_animations[nid];
    std::pair<int, int> insert_loc = FindFrameIdx(nid, time);
    NodeKeyFrame *to_insert = new NodeKeyFrame();
    to_insert->nid = nid;
    to_insert->time = time;
    to_insert->b = basis;

    if (insert_loc.first < 0) {
        frames->insert(frames->begin(), to_insert);
    } else if (insert_loc.second >= frames->size()) {
        frames->push_back(to_insert);
    } else if (insert_loc.first != insert_loc.second) {
        frames->insert(frames->begin() + insert_loc.first, to_insert);
    } else {
        NodeKeyFrame *last = frames->at(insert_loc.first);
        frames->at(insert_loc.first) = to_insert;
        delete last;
    }
}

void Animation::RemoveKeyFrame(uint32_t nid, uint32_t kfid) {
    std::vector<NodeKeyFrame *> *frames = node_animations[nid];
    free(frames->at(kfid));
    frames->erase(frames->begin() + kfid);
}

void Animation::SetAtTime(float time) {
    for (int i = 0; i < model_->NumNodes(); i++) {
        Node *node = model_->GetNode(i);

        std::pair<int, int> loc = FindFrameIdx(i, time);
        if (loc.first < 0 || loc.second >= node_animations[i]->size()) {
            continue;
        } else if (loc.first < loc.second) {
            NodeKeyFrame *prev = node_animations.at(i)->at(loc.first);
            NodeKeyFrame *next = node_animations.at(i)->at(loc.second);

            float weight = (time - prev->time) / (next->time - prev->time);

//            node->pos.x = (prev->pos.x * (1-weight)) + (next->pos.x * (weight));
//            node->pos.y = (prev->pos.y * (1-weight)) + (next->pos.y * (weight));
//            node->pos.z = (prev->pos.z * (1-weight)) + (next->pos.z * (weight));

//            node->angle.x = (prev->angle.x * (1-weight)) + (next->angle.x * weight);
//            node->angle.y = (prev->angle.y * (1-weight)) + (next->angle.y * weight);
//            node->angle.z = (prev->angle.z * (1-weight)) + (next->angle.z * weight);
        } else {
            NodeKeyFrame *curr = node_animations.at(i)->at(loc.first);

            node->b = curr->b;
        }
    }
}

void Animation::FromFile(std::ifstream &file) {
    /*std::string line;
    
    for (int i = 0; i < model_->NumNodes(); i++) {
        getline(file, line);
        int nodeid;
        sscanf(line.c_str(), "nid %d", &nodeid);
        
        getline(file, line);
        int numkeys;
        sscanf(line.c_str(), "%d keys", &numkeys);
        
        std::cout<<i<<" "<<nodeid<<" "<<numkeys<<" keys"<<std::endl;
        
        std::vector<NodeKeyFrame *> *frames = node_animations[i];
        for (int j = 0; j < numkeys; j++) {
            getline(file, line);
            float time, posx, posy, posz, angx, angy, angz;
            
            sscanf(line.c_str(), "%f %f %f %f %f %f %f", &time, &posx, &posy, &posz, &angx, &angy, &angz);
            //SetKeyFrame(i, time, vector_make_float3(posx, posy, posz), vector_make_float3(angx, angy, angz));
        }
    }
    */
}

void Animation::AddToFile(std::ofstream &file) {
    for (int i = 0; i < node_animations.size(); i++) {
        file << "nid " << i << std::endl;
        file << node_animations[i]->size() << " keys" << std::endl;
        for (int j = 0; j < node_animations[i]->size(); j++) {
            file << node_animations[i]->at(j)->time << " ";
//            file << node_animations[i]->at(j)->pos.x << " ";
//            file << node_animations[i]->at(j)->pos.y << " ";
//            file << node_animations[i]->at(j)->pos.z << " ";
//            file << node_animations[i]->at(j)->angle.x << " ";
//            file << node_animations[i]->at(j)->angle.y << " ";
//            file << node_animations[i]->at(j)->angle.z << std::endl;
        }
    }
}

Model::Model(uint32_t mid) : modelID(mid) {
    name_ = "model"+std::to_string(mid);
    face_start = 0;
    vertex_start = 0;
    num_vertices = 0;
    node_start = 0;
    
    Node *node = new Node();
    node->b = Basis();
    nodes.push_back(node);
}

unsigned Model::MakeVertex(float x, float y, float z) {
    num_vertices++;
    
    NodeVertexLink *nvlink = new NodeVertexLink();
    nvlink->nid = 0;
    nvlink->weight = 1;
    nvlink->vector.x = x;
    nvlink->vector.y = y;
    nvlink->vector.z = z;
    nvlinks.push_back(nvlink);
    nvlinks.push_back(new NodeVertexLink()); // empty link
    
    return num_vertices-1;
}

unsigned Model::MakeFace(unsigned v0, unsigned v1, unsigned v2, vector_float4 color) {
    Face *f = new Face();
    f->vertices[0] = v0;
    f->vertices[1] = v1;
    f->vertices[2] = v2;
    f->color = color;
    f->normal_reversed = false;
    f->lighting_offset = {0,0,0};
    f->shading_multiplier = 0.6;
    faces.push_back(f);
    return faces.size()-1;
}

unsigned Model::MakeFaceWithLighting(unsigned v0, unsigned v1, unsigned v2, vector_float4 color, bool normal_reversed, vector_float3 lighting_offset, float shading_multiplier) {
    Face *f = new Face();
    f->vertices[0] = v0;
    f->vertices[1] = v1;
    f->vertices[2] = v2;
    f->color = color;
    f->normal_reversed = normal_reversed;
    f->lighting_offset = lighting_offset;
    f->shading_multiplier = shading_multiplier;
    faces.push_back(f);
    return faces.size()-1;
}

unsigned Model::MakeNode(float x, float y, float z) {
    Node *node = new Node();
    node->b = Basis();
    node->b.pos = vector_make_float3(x, y, z);
    nodes.push_back(node);
    
    for (int i = 0; i < animations.size(); i++) {
        animations[i]->AddNode();
    }
    return nodes.size()-1;
}

void Model::LinkNodeAndVertex(unsigned long vid, unsigned long nid) {
    if (nid > nodes.size() || vid > num_vertices) {
        return;
    }
    
    Vertex vertex = GetVertex(vid);
    Node *node = nodes.at(nid);
    
    NodeVertexLink *link1 = nvlinks[vid*2];
    
    int setIndex = vid*2+1;
    
    if (link1->nid == -1) {
        setIndex = vid*2;
    }
    
    NodeVertexLink *nvlink = nvlinks[setIndex];
    nvlink->nid = nid;
    nvlink->vector.x = vertex.x - node->b.pos.x;
    nvlink->vector.y = vertex.y - node->b.pos.y;
    nvlink->vector.z = vertex.z - node->b.pos.z;
    
//    vector_float3 reverse_angle = vector_make_float3(-node->angle.x, -node->angle.y, -node->angle.z);
//    nvlink->vector = RotateAround(nvlink->vector, vector_make_float3(0, 0, 0), reverse_angle);
    nvlink->vector = TranslatePointToBasis(&node->b, vertex);
    nvlink->weight = 1;
    nvlinks[setIndex] = nvlink;
    
    DetermineLinkWeights(vertex, vid);
}

void Model::UnlinkNodeAndVertex(unsigned long vid, unsigned long nid) {
    Vertex v = GetVertex(vid);
    
    if (nvlinks[vid*2]->nid == nid) {
        nvlinks[vid*2]->nid = -1;
        DetermineLinkWeights(v, vid);
        return;
    } else if (nvlinks[vid*2 + 1]->nid == nid) {
        nvlinks[vid*2 + 1]->nid = -1;
        DetermineLinkWeights(v, vid);
        return;
    }
}

void Model::DetermineLinkWeights(Vertex loc, unsigned long vid) {
    std::vector<unsigned long> links;
    
    NodeVertexLink *link1 = nvlinks.at(vid*2);
    NodeVertexLink *link2 = nvlinks.at(vid*2 + 1);
    
    if (link1->nid == -1 && link2->nid == -1) {
        return;
    } else if (link1->nid == -1) {
        link2->weight = 1;
    } else if (link2->nid == -1) {
        link1->weight = 1;
    } else {
        float link1mag = sqrt(pow(link1->vector.x, 2) + pow(link1->vector.y, 2) + pow(link1->vector.z, 2));
        float link2mag = sqrt(pow(link2->vector.x, 2) + pow(link2->vector.y, 2) + pow(link2->vector.z, 2));
        
        if (link1mag == 0) {
            link1->weight = 1;
            link2->weight = 0;
        } else if (link2mag == 0) {
            link1->weight = 0;
            link2->weight = 1;
        } else {
            float inverse_dist_sum = 1/link1mag + 1/link2mag;
            link1->weight = (1/link1mag) / inverse_dist_sum;
            link2->weight = (1/link2mag) / inverse_dist_sum;
        }
    }
}

void Model::LockNodeToNode(unsigned nid1, unsigned nid2) {
    nodes[nid1]->locked_to = nid2;
}

void Model::MakeCube() {
    MakeVertex(0, 0, 0);
    MakeVertex(1, 0, 0);
    MakeVertex(0, 1, 0);
    MakeVertex(1, 1, 0);
    MakeVertex(0, 0, 1);
    MakeVertex(1, 0, 1);
    MakeVertex(0, 1, 1);
    MakeVertex(1, 1, 1);
    
    MakeFace(1, 0, 2, {1, 1, 1, 1});
    MakeFace(2, 3, 1, {1, 1, 1, 1});
    
    MakeFace(1, 0, 4, {1, 1, 1, 1});
    MakeFace(4, 5, 1, {1, 1, 1, 1});
    
    MakeFace(2, 0, 4, {1, 1, 1, 1});
    MakeFace(2, 6, 4, {1, 1, 1, 1});
    
    MakeFace(3, 2, 6, {1, 1, 1, 1});
    MakeFace(3, 7, 6, {1, 1, 1, 1});
    
    MakeFace(3, 1, 5, {1, 1, 1, 1});
    MakeFace(5, 7, 3, {1, 1, 1, 1});
    
    MakeFace(5, 4, 6, {1, 1, 1, 1});
    MakeFace(5, 7, 6, {1, 1, 1, 1});
}

void Model::MakeRect(float w, float l, float h) {
    MakeVertex(0, 0, 0);
    MakeVertex(w, 0, 0);
    MakeVertex(0, l, 0);
    MakeVertex(w, l, 0);
    MakeVertex(0, 0, h);
    MakeVertex(w, 0, h);
    MakeVertex(0, l, h);
    MakeVertex(w, l, h);
    
    MakeFace(1, 0, 2, {1, 1, 1, 1});
    MakeFace(2, 3, 1, {1, 1, 1, 1});
    
    MakeFace(1, 0, 4, {1, 1, 1, 1});
    MakeFace(4, 5, 1, {1, 1, 1, 1});
    
    MakeFace(2, 0, 4, {1, 1, 1, 1});
    MakeFace(2, 6, 4, {1, 1, 1, 1});
    
    MakeFace(3, 2, 6, {1, 1, 1, 1});
    MakeFace(3, 7, 6, {1, 1, 1, 1});
    
    MakeFace(3, 1, 5, {1, 1, 1, 1});
    MakeFace(5, 7, 3, {1, 1, 1, 1});
    
    MakeFace(5, 4, 6, {1, 1, 1, 1});
    MakeFace(5, 7, 6, {1, 1, 1, 1});
}

void Model::MakeCube(vector_float4 color) {
    MakeVertex(0, 0, 0);
    MakeVertex(1, 0, 0);
    MakeVertex(0, 1, 0);
    MakeVertex(1, 1, 0);
    MakeVertex(0, 0, 1);
    MakeVertex(1, 0, 1);
    MakeVertex(0, 1, 1);
    MakeVertex(1, 1, 1);
    
    MakeFace(1, 0, 2, color);
    MakeFace(2, 3, 1, color);
    
    MakeFace(1, 0, 4, color);
    MakeFace(4, 5, 1, color);
    
    MakeFace(2, 0, 4, color);
    MakeFace(2, 6, 4, color);
    
    MakeFace(3, 2, 6, color);
    MakeFace(3, 7, 6, color);
    
    MakeFace(3, 1, 5, color);
    MakeFace(5, 7, 3, color);
    
    MakeFace(5, 4, 6, color);
    MakeFace(5, 7, 6, color);
}

void Model::MakeRect(float w, float l, float h, vector_float4 color) {
    MakeVertex(0, 0, 0);
    MakeVertex(w, 0, 0);
    MakeVertex(0, l, 0);
    MakeVertex(w, l, 0);
    MakeVertex(0, 0, h);
    MakeVertex(w, 0, h);
    MakeVertex(0, l, h);
    MakeVertex(w, l, h);
    
    MakeFace(1, 0, 2, color);
    MakeFace(2, 3, 1, color);
    
    MakeFace(1, 0, 4, color);
    MakeFace(4, 5, 1, color);
    
    MakeFace(2, 0, 4, color);
    MakeFace(2, 6, 4, color);
    
    MakeFace(3, 2, 6, color);
    MakeFace(3, 7, 6, color);
    
    MakeFace(3, 1, 5, color);
    MakeFace(5, 7, 3, color);
    
    MakeFace(5, 4, 6, color);
    MakeFace(5, 7, 6, color);
}

void Model::MakePlatform(float w, float l, vector_float4 color) {
    MakeVertex(0, 0, 0);
    MakeVertex(w, 0, 0);
    MakeVertex(0, l, 0);
    MakeVertex(w, l, 0);
    
    MakeFace(1, 0, 2, color);
    MakeFace(2, 3, 1, color);
}

void Model::InsertVertex(float x, float y, float z, int vid) {
    if (vid >= num_vertices) {
        MakeVertex(x, y, z);
    } else {
        NodeVertexLink *nvlink = new NodeVertexLink();
        num_vertices++;
        nvlink->nid = 0;
        nvlink->weight = 1;
        nvlink->vector.x = x;
        nvlink->vector.y = y;
        nvlink->vector.z = z;
        nvlinks.insert(nvlinks.begin()+vid*2, nvlink);
        nvlinks.insert(nvlinks.begin()+vid*2 + 1, new NodeVertexLink());
    }
}

void Model::InsertFace(Face *face, int fid) {
    if (fid >= faces.size()) {
        faces.push_back(face);
    } else {
        faces.insert(faces.begin()+fid, face);
    }
}

void Model::MoveVertexBy(unsigned vid, float dx, float dy, float dz) {
    nvlinks[vid*2]->vector.x += dx;
    nvlinks[vid*2]->vector.y += dy;
    nvlinks[vid*2]->vector.z += dz;
    
    nvlinks[vid*2 + 1]->vector.x += dx;
    nvlinks[vid*2 + 1]->vector.y += dy;
    nvlinks[vid*2 + 1]->vector.z += dz;
}

void Model::MoveNodeBy(unsigned nid, float dx, float dy, float dz) {
    float new_x = nodes[nid]->b.pos.x + dx;
    float new_y = nodes[nid]->b.pos.y + dy;
    float new_z = nodes[nid]->b.pos.z + dz;
    
    if (nodes[nid]->locked_to == -1) {
        nodes[nid]->b.pos.x = new_x;
        nodes[nid]->b.pos.y = new_y;
        nodes[nid]->b.pos.z = new_z;
    } else {
        float curr_dist = dist3to3(nodes[nid]->b.pos, nodes[nodes[nid]->locked_to]->b.pos);
        float new_dist = dist3to3(vector_make_float3(new_x, new_y, new_z), nodes[nodes[nid]->locked_to]->b.pos);
        nodes[nid]->b.pos.x = new_x * curr_dist / new_dist;
        nodes[nid]->b.pos.y = new_y * curr_dist / new_dist;
        nodes[nid]->b.pos.z = new_z * curr_dist / new_dist;
    }
}

void Model::MoveVertexTo(unsigned vid, float x, float y, float z) {
    int nid1 = nvlinks[vid*2]->nid;
    float dx = x - (nodes[nid1]->b.pos.x + nvlinks[vid*2]->vector.x);
    float dy = y - (nodes[nid1]->b.pos.y + nvlinks[vid*2]->vector.y);
    float dz = z - (nodes[nid1]->b.pos.z + nvlinks[vid*2]->vector.z);
    
    nvlinks[vid*2]->vector.x += dx;
    nvlinks[vid*2]->vector.y += dy;
    nvlinks[vid*2]->vector.z += dz;
    
    nvlinks[vid*2 + 1]->vector.x += dx;
    nvlinks[vid*2 + 1]->vector.y += dy;
    nvlinks[vid*2 + 1]->vector.z += dz;
}

void Model::MoveNodeTo(unsigned nid, float x, float y, float z) {
    if (nodes[nid]->locked_to == -1) {
        nodes[nid]->b.pos.x = x;
        nodes[nid]->b.pos.y = y;
        nodes[nid]->b.pos.z = z;
    } else {
        float curr_dist = dist3to3(nodes[nid]->b.pos, nodes[nodes[nid]->locked_to]->b.pos);
        float new_dist = dist3to3(vector_make_float3(x, y, z), nodes[nodes[nid]->locked_to]->b.pos);
        nodes[nid]->b.pos.x = x * new_dist / curr_dist;
        nodes[nid]->b.pos.y = y * new_dist / curr_dist;
        nodes[nid]->b.pos.z = z * new_dist / curr_dist;
    }
}

void Model::RemoveVertex(int vid) {
    for (int i = faces.size()-1; i >= 0; i--) {
        Face *f = faces[i];
        
        if (f->vertices[0] == vid || f->vertices[1] == vid || f->vertices[2] == vid) {
            RemoveFace(i);
            continue;
        }
        
        if (f->vertices[0] > vid) {
            f->vertices[0]--;
        }
        if (f->vertices[1] > vid) {
            f->vertices[1]--;
        }
        if (f->vertices[2] > vid) {
            f->vertices[2]--;
        }
    }
    
    nvlinks.erase(nvlinks.begin() + vid*2 + 1);
    nvlinks.erase(nvlinks.begin() + vid*2);
    num_vertices --;
}

void Model::RemoveFace(int fid) {
    if (fid < faces.size()) {
        delete faces[fid];
        faces.erase(faces.begin() + fid);
    }
}

void Model::ShiftVertices(float dx, float dy, float dz) {
    for (int i = 0; i < nvlinks.size(); i++) {
        nvlinks[i]->vector.x += dx;
        nvlinks[i]->vector.y += dy;
        nvlinks[i]->vector.z += dz;
    }
}

unsigned Model::MakeAnimation() {
    Animation *animation = new Animation(this);
    animations.push_back(animation);
    return animations.size()-1;
}

void Model::StartAnimation(int aid) {
    curr_aid = aid;
    curr_anim_time = 0;
}

void Model::SetKeyFrame(int aid, int nid, float time) {
    Animation *anim = animations.at(aid);
    Node *n = nodes.at(nid);
    
    anim->SetKeyFrame(nid, time, n->b);
}

void Model::UpdateAnimation(float dt) {
    if (curr_aid >= 0) {
        curr_anim_time += dt;
        //std::cout<<curr_anim_time<<std::endl;
        Animation *anim = animations.at(curr_aid);
        if (curr_anim_time > anim->GetLength()) {
            curr_aid = -1;
            curr_anim_time = 0;
        } else {
            anim->SetAtTime(curr_anim_time);
        }
    } else {
        curr_anim_time = 0;
    }
}

unsigned Model::NumAnimations() {
    return animations.size();
}

Vertex Model::GetVertex(unsigned long vid) {
    Vertex ret = vector_make_float3(0, 0, 0);
    
    NodeVertexLink *link1 = nvlinks[vid*2];
    NodeVertexLink *link2 = nvlinks[vid*2 + 1];
    
    if (link1->nid != -1) {
        Node *n = nodes[link1->nid];
        
//        Vertex desired1 = vector_make_float3(n->pos.x + link1->vector.x, n->pos.y + link1->vector.y, n->pos.z + link1->vector.z);
//        desired1 = RotateAround(desired1, n->pos, n->b);
        
        Vertex desired1 = TranslatePointToStandard(&n->b, link1->vector);
        
        ret.x += link1->weight*desired1.x;
        ret.y += link1->weight*desired1.y;
        ret.z += link1->weight*desired1.z;
    }
    
    if (link2->nid != -1) {
        Node *n = nodes[link2->nid];
        
//        Vertex desired2 = vector_make_float3(n->pos.x + link2->vector.x, n->pos.y + link2->vector.y, n->pos.z + link2->vector.z);
//        desired2 = TranslatePoint(desired2, n->pos, n->b);
        
        Vertex desired2 = TranslatePointToStandard(&n->b, link2->vector);
        
        ret.x += link2->weight*desired2.x;
        ret.y += link2->weight*desired2.y;
        ret.z += link2->weight*desired2.z;
    }
    
    return ret;
}

Face *Model::GetFace(unsigned long fid) {
    return faces.at(fid);
}

std::vector<unsigned long> Model::GetEdgeFaces(unsigned long vid1, unsigned long vid2) {
    std::vector<unsigned long> ret;
    
    for (std::size_t fid = 0; fid < faces.size(); fid++) {
        Face *f = faces[fid];
        if (f->vertices[0] == vid1 || f->vertices[1] == vid1 || f->vertices[2] == vid1) {
            if (f->vertices[0] == vid2 || f->vertices[1] == vid2 || f->vertices[2] == vid2) {
                ret.push_back(fid);
            }
        }
    }
    
    return ret;
}

bool Model::HasFaceWith(std::vector<int> &vids) {
    for (std::size_t fid = 0; fid < faces.size(); fid++) {
        Face *f = faces[fid];
        int numwith = 0;
        for (int i = 0; i < vids.size(); i++) {
            if (f->vertices[0] == vids[i] || f->vertices[1] == vids[i] || f->vertices[2] == vids[i]) {
                numwith++;
            }
        }
        
        if (numwith >= 3) {
            return true;
        }
    }
    
    return false;
}

std::vector<unsigned long> Model::GetLinkedNodes(unsigned long vid) {
    std::vector<unsigned long> ret;
    
    NodeVertexLink *link1 = nvlinks[vid*2];
    NodeVertexLink *link2 = nvlinks[vid*2 + 1];
    
    if (link1->nid != -1) {
        ret.push_back(link1->nid);
    }
    
    if (link2->nid != -1) {
        ret.push_back(link2->nid);
    }
    
    return ret;
}

std::vector<unsigned long> Model::GetLinkedVertices(unsigned long nid) {
    std::vector<unsigned long> ret;
    for (int i = 0; i < nvlinks.size(); i++) {
        if (nvlinks[i]->nid == nid) {
            ret.push_back(i/2);
        }
    }
    
    return ret;
}

Node *Model::GetNode(unsigned long nid) {
    return nodes.at(nid);
}

std::vector<Face *> &Model::GetFaces() {
    return faces;
}

std::vector<Node *> &Model::GetNodes() {
    return nodes;
}

void Model::AddToBuffers(std::vector<Face> &faceBuffer, std::vector<Node> &nodeBuffer, std::vector<NodeVertexLink> &nvlinkBuffer, std::vector<uint32_t> &node_modelIDs, unsigned &total_vertices) {
    face_start = faceBuffer.size();
    vertex_start = total_vertices;
    
    total_vertices += num_vertices;
    
    for (int i = 0; i < faces.size(); i++) {
        Face *og = faces[i];
        Face face;
        face.color = og->color;
        face.vertices[0] = og->vertices[0]+vertex_start;
        face.vertices[1] = og->vertices[1]+vertex_start;
        face.vertices[2] = og->vertices[2]+vertex_start;
        face.normal_reversed = og->normal_reversed;
        face.lighting_offset = og->lighting_offset;
        face.shading_multiplier = og->shading_multiplier;
        faceBuffer.push_back(face);
    }
    
    node_start = nodeBuffer.size();
    
    for (int i = 0; i < nodes.size(); i++) {
        Node *og = nodes[i];
        Node node;
        node.b = og->b;
        nodeBuffer.push_back(node);
        node_modelIDs.push_back(modelID);
    }
    
    for (int i = 0; i < nvlinks.size(); i++) {
        NodeVertexLink *og = nvlinks[i];
        NodeVertexLink nvlink;
        if (og->nid != -1) {
            nvlink.nid = og->nid + node_start;
        } else {
            nvlink.nid = -1;
        }
        nvlink.vector = og->vector;
        nvlink.weight = og->weight;
        nvlinkBuffer.push_back(nvlink);
    }
}

void Model::UpdateNodeBuffers(std::vector<Node> &nodeBuffer) {
    for (int i = 0; i < nodes.size(); i++) {
        Node *node = &nodeBuffer.at(i+node_start);
        node->b = nodes[i]->b;
    }
}

uint32_t Model::ModelID() {
    return modelID;
}

void Model::SetFaceStart(unsigned long n) {
    face_start = n;
}

void Model::SetVertexStart(unsigned long n) {
    vertex_start = n;
}

unsigned long Model::FaceStart() {
    return face_start;
}

unsigned long Model::VertexStart() {
    return vertex_start;
}

unsigned long Model::NodeStart() {
    return node_start;
}

unsigned long Model::NumFaces() {
    return faces.size();
}

unsigned long Model::NumVertices() {
    return num_vertices;
}

unsigned long Model::NumNodes() {
    return nodes.size();
}

void Model::SetId(uint32_t mid) {
    modelID = mid;
}

void Model::SetName(std::string name) {
    name_ = name;
}

std::string Model::GetName() {
    return name_;
}

void Model::SaveToFile(std::string path) {
    std::ofstream myfile;
    myfile.open (path + name_ + ".drgn");
    
    myfile << nodes.size() << " nodes" << std::endl;
    for (int i = 0; i < nodes.size(); i++) {
        Node *n = nodes[i];
        DragonflyUtils::BasisToFile(myfile, &n->b);
    }
    
    myfile << nvlinks.size() << " vertices" << std::endl;
    for (int i = 0; i < nvlinks.size(); i++) {
        NodeVertexLink *nv = nvlinks[i];
        myfile << nv->nid << " ";
        myfile << nv->weight << " ";
        myfile << nv->vector.x << " " << nv->vector.y << " " << nv->vector.z << std::endl;
    }
    
    myfile << faces.size() << " faces" << std::endl;
    for (int i = 0; i < faces.size(); i++) {
        Face *f = faces[i];
        myfile << f->vertices[0] << " " << f->vertices[1] << " " << f->vertices[2] << " ";
        myfile << f->color.x << " " << f->color.y << " " << f->color.z << " " << f->color.w << " ";
        myfile << f->normal_reversed << " " << f->shading_multiplier << " ";
        myfile << f->lighting_offset.x << " " << f->lighting_offset.y << " " << f->lighting_offset.z << std::endl;
    }
    
    myfile << animations.size() << " animations" << std::endl;
    for (int i = 0; i < animations.size(); i++) {
        Animation *a = animations[i];
        a->AddToFile(myfile);
    }
    
    myfile.close();
}

void Model::FromFile(std::string path) {
    /*std::string line;
    std::ifstream myfile (path);
    std::cout<<fs::current_path()<<std::endl;
    if (myfile.is_open()) {
        getline(myfile, line);
        delete nodes[0];
        nodes.erase(nodes.begin());
        
        int num_nodes;
        sscanf(line.c_str(), "%d nodes", &num_nodes);
        for (int i = 0; i < num_nodes; i++) {
            Node *n = new Node();
            n->b = DragonflyUtils::BasisFromFile(myfile);
            n->locked_to = -1;
            
            nodes.push_back(n);
        }
        
        getline(myfile, line);
        int num_links;
        sscanf(line.c_str(), "%d vertices", &num_links);
        for (int i = 0; i < num_links; i++) {
            getline(myfile, line);
            NodeVertexLink *nvlink = new NodeVertexLink();
            int nid;
            float nvweight, posx, posy, posz;
            sscanf(line.c_str(), "%d %f %f %f %f", &nid, &nvweight, &posx, &posy, &posz);
            nvlink->nid = nid;
            nvlink->weight = nvweight;
            nvlink->vector = vector_make_float3(posx, posy, posz);
            
            nvlinks.push_back(nvlink);
        }
        num_vertices = num_links / 2;
        
        getline(myfile, line);
        int num_faces;
        sscanf(line.c_str(), "%d faces", &num_faces);
        for (int i = 0; i < num_faces; i++) {
            getline(myfile, line);
            Face *f = new Face();
            int v1, v2, v3;
            float cx, cy, cz, cw;
            int nr;
            float sm;
            float lx, ly, lz;
            sscanf(line.c_str(), "%d %d %d %f %f %f %f %d %f %f %f %f", &v1, &v2, &v3, &cx, &cy, &cz, &cw, &nr, &sm, &lx, &ly, &lz);
            f->vertices[0] = v1;
            f->vertices[1] = v2;
            f->vertices[2] = v3;
            f->color = vector_make_float4(cx, cy, cz, cw);
            f->normal_reversed = nr == 1;
            f->shading_multiplier = sm;
            f->lighting_offset = vector_make_float3(lx, ly, lz);
            
            faces.push_back(f);
        }
        
        getline(myfile, line);
        int num_anims;
        sscanf(line.c_str(), "%d animations", &num_anims);
        for (int i = 0; i < num_anims; i++) {
            //getline(myfile, line);
            Animation *anim = new Animation(this);
            
            anim->FromFile(myfile);
            animations.push_back(anim);
        }
        
        myfile.close();
        
        std::cout<<NumVertices()<<std::endl;
        std::cout<<NumFaces()<<std::endl;
    } else {
        std::cout<<"invalid path"<<std::endl;
    }
    */
}

Model::~Model() {
}

