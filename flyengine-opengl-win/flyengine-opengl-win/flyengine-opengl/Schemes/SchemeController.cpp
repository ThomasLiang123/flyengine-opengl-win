//
//  SchemeController.cpp
//  flyengine-test
//
//  Created by Thomas Liang on 10/24/23.
//

#include "SchemeController.h"

SchemeController::SchemeController(Scheme *scheme) {
    scheme_ = scheme;
}

SchemeController::~SchemeController() {
    
}

void SchemeController::BuildUI() {
}

void SchemeController::SetScheme(Scheme *scheme) {
    scheme_ = scheme;
}

Scheme *SchemeController::GetScheme() {
    return scheme_;
}
