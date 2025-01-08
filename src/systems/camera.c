/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : camera
 * @created     : Samedi jan 04, 2025 16:12:05 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/affine.h"
#include "cglm/io.h"
#include "cglm/vec3.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "gfx.h"
#include "shader.h"
#include "systems.h"

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static void create_projection_matrix(float screen_width, float screen_height, mat4 proj_matrix);
static void create_view_matrix(float height, transform_t *transform, mat4 view_matrix);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_camera_init(ecs_entity_t *it, int count, void *args[])
{
    camera_t *camera;
    ecs_get_component(it[count - 1], camera_t, &camera);

    if (camera->zoom == 0) camera->zoom = 10;

    // Load uniform
    mat4 projection;
    create_projection_matrix(SCR_WIDTH, SCR_HEIGHT, projection);

    shader_use(SHADER_WORLD);
    GLint projection_loc = glGetUniformLocation(shader_get_program(SHADER_WORLD), "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    shader_use(SHADER_TEXT);
    projection_loc = glGetUniformLocation(shader_get_program(SHADER_TEXT), "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    return ECS_OK;
}

ecs_err_t system_camera_update(ecs_entity_t *it, int count, void *args[])
{
    camera_t *camera;
    transform_t *transform;
    ecs_get_component(it[count - 1], camera_t, &camera);
    ecs_get_component(it[count - 1], transform_t, &transform);

    shader_use(SHADER_WORLD);
    int grayscale_loc = glGetUniformLocation(shader_get_program(SHADER_WORLD), "u_GrayscaleAmount");
    glUniform1f(grayscale_loc, camera->color_filter_strength);

    if (camera->detach) return ECS_OK;

    mat4 view;
    create_view_matrix(camera->zoom, transform, view);

    GLint view_loc = glGetUniformLocation(shader_get_program(SHADER_WORLD), "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    shader_use(SHADER_TEXT);
    view_loc = glGetUniformLocation(shader_get_program(SHADER_TEXT), "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    return ECS_OK;
}

inline void create_projection_matrix(float screen_width, float screen_height, mat4 proj_matrix)
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

inline void create_view_matrix(float height, transform_t *transform, mat4 view_matrix)
{
    glm_mat4_identity(view_matrix);

    view_matrix[0][0] = 2.0f / height;
    view_matrix[1][1] = 2.0f / height;

    vec3 pos;
    glm_vec3_scale(transform->position, -1.0f, pos);
    glm_translate(view_matrix, pos);
}
