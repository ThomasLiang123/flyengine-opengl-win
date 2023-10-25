//
//  SchemeController.h
//  flyengine-test
//
//  Created by Thomas Liang on 10/24/23.
//

#ifndef SchemeController_h
#define SchemeController_h

#include "imgui.h"

#include "Scheme.h"
#include "ViewScheme.h"

class SchemeController {
private:
    Scheme *scheme_;
public:
    SchemeController(Scheme *scheme);
    ~SchemeController();
    
    void BuildUI();
    
    void SetScheme(Scheme *scheme);
    Scheme *GetScheme();
};

#endif /* SchemeController_h */
