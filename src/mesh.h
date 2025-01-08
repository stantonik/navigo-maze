/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : mesh
 * @created     : Mercredi jan 08, 2025 12:11:01 CET
 */

#ifndef MESH_H
#define MESH_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "components.h"
#include "GL/glew.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define ATTRIBUTE_POSITION 0
#define ATTRIBUTE_MODEL_MAT_ROW0 1
#define ATTRIBUTE_MODEL_MAT_ROW1 2
#define ATTRIBUTE_MODEL_MAT_ROW2 3
#define ATTRIBUTE_MODEL_MAT_ROW3 4
#define ATTRIBUTE_COLOR 5
#define ATTRIBUTE_UVOFFSET 6
#define ATTRIBUTE_UVSCALE 7

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct
{
    mat4 model_mat;
    vec4 color;
    vec2 uvoffset;
    vec2 uvscale;
} instance_t;

static const vec3 quad_vertices[6] =
{
    { 1, 1, 0 }, // top-right
    { 0, 1, 0 }, // top-left
    { 1, 0, 0 }, // bottom-right
    { 1, 0, 0 }, // bottom-right
    { 0, 1, 0 }, // top-left
    { 0, 0, 0 }  // bottom-left
};

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
void mesh_instance_init(GLuint *VAO, GLuint *VBO, GLuint *IBO, size_t IBO_size);

//------------------------------------------------------------------------------
// Inlined Function
//------------------------------------------------------------------------------
static inline void create_model_matrix(transform_t *transform, mat4 model)
{
    mat4 scale_matrix;
    mat4 rotation_matrix;
    mat4 translation_matrix;

    // Initialize matrices
    glm_mat4_identity(scale_matrix);
    glm_mat4_identity(rotation_matrix);
    glm_mat4_identity(translation_matrix);

    vec2 scale;
    glm_vec3_scale(transform->scale, 1.01f, scale); // avoid gap glitchs between tiles
    glm_scale(scale_matrix, scale);

    /* glm_rotate(rotation_matrix, glm_rad(transform->rotation[0]), (vec3){1.0f, 0.0f, 0.0f}); */
    /* glm_rotate(rotation_matrix, glm_rad(transform->rotation[1]), (vec3){0.0f, 1.0f, 0.0f}); */
    /* glm_rotate(rotation_matrix, glm_rad(transform->rotation[2]), (vec3){0.0f, 0.0f, 1.0f}); */

    // Center the sprite (adjust for bottom-left reference point)
    glm_translate(translation_matrix, transform->position); 

    glm_mat4_mul(translation_matrix, rotation_matrix, model);
    glm_mat4_mul(model, scale_matrix, model);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MESH_H */

