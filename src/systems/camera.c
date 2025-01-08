/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : camera
 * @created     : Samedi jan 04, 2025 16:12:05 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "camera.h"
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
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_camera_init(ecs_entity_t *it, int count, void *args[])
{
    camera_t *camera;
    ecs_get_component(it[count - 1], camera_t, &camera);

    if (camera->zoom == 0) camera->zoom = 10;

    // Load uniform
    create_projection_matrix(SCR_WIDTH, SCR_HEIGHT, camera->projection);

    shader_use(SHADER_WORLD);
    GLint projection_loc = glGetUniformLocation(shader_get_program(SHADER_WORLD), "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)camera->projection);

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

    create_view_matrix(camera->zoom, transform, camera->view);

    GLint view_loc = glGetUniformLocation(shader_get_program(SHADER_WORLD), "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)camera->view);

    return ECS_OK;
}
