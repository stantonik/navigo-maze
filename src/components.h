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

typedef struct
{
    GLuint VAO, VBO, EBO;
} mesh_t;

typedef struct
{
    const char *name;
    int width, height;
} texture_t;

typedef struct 
{ 

} controller_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* COMPONENTS_H */

