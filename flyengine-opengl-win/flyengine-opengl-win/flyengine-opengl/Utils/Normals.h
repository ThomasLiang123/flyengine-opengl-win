//
//  GenerateLighting.h
//  dragonfly
//
//  Created by Thomas Liang on 10/24/23.
//

#ifndef GenerateLighting_h
#define GenerateLighting_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include "Utils/Vec.h"
#include <iostream>

#include "../Modeling/Model.h"
#include "../Modeling/Scene.h"
#include "Utils.h"

namespace DragonflyUtils {
int FaceIntercept(Model *m, vector_float3 start, vector_float3 vector, int avoid);
void FindNormals(Model *m);
void ReverseNormals(Model *m);
}

#endif /* GenerateLighting_h */
