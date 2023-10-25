//
//  Camera.h
//  flyengine-test
//
//  Created by Thomas Liang on 10/15/23.
//

#ifndef Camera_h
#define Camera_h

struct Camera {
    vector_float3 pos;
    vector_float3 vector;
    vector_float3 up_vector;
    vector_float2 FOV;
};

#endif /* Camera_h */
