/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : components
 * @created     : Vendredi jan 03, 2025 19:08:44 CET
 */

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "ecs/ecs.h"
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/types.h"
#include "GL/glew.h"
#include "miniaudio.h"
#include "stdbool.h"
#include "utils/vector.h"

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
    float friction;
    float mass;
    bool gravity;
} rigidbody_t;

typedef struct
{
    bool is_trigger;
    vec3 offset;
    vec3 size;
    vec3 impact_position;
    vec3 impact_normal;
    vector_t entities;
} rect_collider_t;

typedef struct
{
    char texture_name[32];
    int width, height;
    vec4 color;
    vec2 uv_min, uv_max;
} sprite_t;

typedef struct
{
    char text[256];
    vec4 color;
    float size;
    ecs_entity_t camera;
} text_t;

typedef struct
{
    char name[32];
    float volume;
    bool loop;
    bool playing;
    bool restart;
    ma_sound sound;
    bool init;
} audio_t;

typedef struct 
{ 
    float walk_speed;
    float run_speed;
    float jump_height;
} controller_t;

typedef struct
{
    int dummy;
} enemy_t;

typedef struct
{
    float zoom;
    float color_filter_strength;
    bool detach;
    mat4 view;
    mat4 projection;
} camera_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* COMPONENTS_H */

