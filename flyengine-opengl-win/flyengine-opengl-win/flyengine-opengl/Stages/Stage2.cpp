//
//  Stage2.cpp
//  flyengine-test
//
//  Created by Thomas Liang on 10/24/23.
//

#include "Stage2.h"
# define PI_2           3.14159265358979323846/2

void Stage2::SetCamera(Camera *c) {
    Stage::SetCamera(c);
    
    camera_->pos = {-2, 0, 0};
    camera_->vector = {1, 0, 0};
    camera_->up_vector = {0, 0, 1};
    camera_->FOV = {PI_2, PI_2};
}

Scene *Stage2::GenerateScene() {
    scene_ = new Scene();
    scene_->MoveModelTo(0, 0, 0, -1);
    should_recalculate_all = true;
    return scene_;
}
