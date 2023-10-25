//
//  EditModelScheme.h
//  flyengine-test
//
//  Created by Thomas Liang on 10/24/23.
//

#ifndef EditModelScheme_h
#define EditModelScheme_h

#include "Scheme.h"

class ViewScheme : public Scheme {
private:
    void HandleMouseUp(vector_float2 loc, bool left);
    void MainWindow();
public:
    ViewScheme();
    ~ViewScheme();
    
    void BuildUI();
    void HandleMouseMovement(float x, float y, float dx, float dy);
    
    virtual void Update();
};

#endif /* EditModelScheme_h */
