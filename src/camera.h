/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : camera
 * @created     : Mercredi jan 08, 2025 14:36:14 CET
 */

#ifndef CAMERA_H
#define CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "components.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
static inline void create_projection_matrix(float screen_width, float screen_height, mat4 proj_matrix)
{
    glm_mat4_identity(proj_matrix);
    
    float aspect_ratio = screen_width / screen_height;
    
    // If aspect ratio > 1 (wide screen), adjust x axis
    // If aspect ratio < 1 (tall screen), adjust y axis
    if (aspect_ratio > 1.0f) 
    {
        proj_matrix[0][0] = 1.0f / aspect_ratio;
    } 
    else 
    {
        proj_matrix[1][1] = aspect_ratio;
    }
}

static inline void create_view_matrix(float height, transform_t *transform, mat4 view_matrix)
{
    glm_mat4_identity(view_matrix);

    view_matrix[0][0] = 2.0f / height;
    view_matrix[1][1] = 2.0f / height;

    vec3 pos;
    glm_vec3_scale(transform->position, -1.0f, pos);
    glm_translate(view_matrix, pos);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CAMERA_H */

