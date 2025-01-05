/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : components
 * @created     : Vendredi jan 03, 2025 19:08:44 CET
 */

#ifndef COMPONENTS_H
#define COMPONENTS_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "GL/glew.h"
#include "cglm/types.h"
#include "stdbool.h"

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct
{
    vec3 position;
    vec3 rotation;
    vec3 scale;
} transform_t;

typedef struct
{
    vec3 velocity;
    float mass;
    bool gravity;
} rigidbody_t;

typedef enum
{
    COLLIDER_SQUARE = 1,
} collider_shape_t;

typedef struct
{
    collider_shape_t shape;
    bool is_trigger;
    vec3 offset;
    vec3 size;
    vec3 impact_position;
    vec3 impact_normal;
} collider_t;

typedef struct
{
    vec3 position;
    vec2 uv;
} vertex_t;

typedef struct
{
    GLuint VAO, VBO, EBO;
    vertex_t vertices[4];
} mesh_t;

typedef struct
{
    char name[32];
    int width, height;
    vec2 uv_min, uv_max;
} texture_t;

typedef struct
{
    char text[256];
    vec3 color;
    float size;
} text_t;

typedef struct 
{ 
    float walk_speed;
    float run_speed;
    float jump_height;
} controller_t;

typedef struct
{
    uint8_t zoom;
} camera_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* COMPONENTS_H */

