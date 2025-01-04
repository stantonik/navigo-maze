/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : mesh
 * @created     : Vendredi jan 03, 2025 20:42:44 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/vec3.h"
#include "cglm/vec2.h"
#include "components.h"
#include "ecs/ecs_err.h"
#include "gfx.h"
#include "systems.h"

#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static const vec3 vertex_pos_unit[4] =
{
    { 1, 1, 0 }, // top-right
    { 1, 0, 0 }, // bottom-right
    { 0, 1, 0 }, // top-left
    { 0, 0, 0 }  // bottom-left
};

static const unsigned int indices[] = 
{
    0, 1, 2,
    2, 1, 3 
};

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_mesh_init(ecs_entity_t *it, int count, void *args)
{
    glUseProgram(gfx_get_shader_program());
    glEnable(GL_DEPTH_TEST);

    for (int i = 0; i < count; ++i)
    {
        mesh_t *mesh;
        transform_t *transform;
        ecs_get_component(it[i], mesh_t, &mesh);
        ecs_get_component(it[i], transform_t, &transform);

        for (int i = 0; i < 4; i++) 
        {
            glm_vec3_mul((float *)vertex_pos_unit[i], transform->scale, mesh->vertices[i].position);
            glm_vec3_add(mesh->vertices[i].position, transform->position, mesh->vertices[i].position);
        }

        glGenVertexArrays(1, &mesh->VAO);
        glGenBuffers(1, &mesh->VBO);
        glGenBuffers(1, &mesh->EBO);

        glBindVertexArray(mesh->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->vertices), mesh->vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)0);
        glEnableVertexAttribArray(0);
        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, uv));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);

        /* glActiveTexture(GL_TEXTURE0); */
        /* glBindTexture(GL_TEXTURE_2D, renderer->textures[0]); */

    }
    return ECS_OK;
}

ecs_err_t system_mesh_draw(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        mesh_t *mesh;
        ecs_get_component(it[i], mesh_t, &mesh);

        glBindVertexArray(mesh->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    return ECS_OK;
}

ecs_err_t system_mesh_update(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        mesh_t *mesh;
        transform_t *transform;
        ecs_get_component(it[i], mesh_t, &mesh);
        ecs_get_component(it[i], transform_t, &transform);

        for (int i = 0; i < 4; i++) 
        {
            glm_vec3_mul((float *)vertex_pos_unit[i], transform->scale, mesh->vertices[i].position);
            glm_vec3_add(mesh->vertices[i].position, transform->position, mesh->vertices[i].position);
        }

        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mesh->vertices), mesh->vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return ECS_OK;
}

ecs_err_t system_mesh_texture_update(ecs_entity_t *it, int count, void *args)
{
    glUseProgram(gfx_get_shader_program());

    for (int i = 0; i < count; ++i)
    {
        mesh_t *mesh;
        texture_t *tex;
        ecs_get_component(it[i], mesh_t, &mesh);
        ecs_get_component(it[i], texture_t, &tex);

        glm_vec2_copy(tex->uv_max, mesh->vertices[0].uv);
        glm_vec2_copy((vec2){ tex->uv_max[0], tex->uv_min[1] }, mesh->vertices[1].uv);
        glm_vec2_copy((vec2){ tex->uv_min[0], tex->uv_max[1] }, mesh->vertices[2].uv);
        glm_vec2_copy(tex->uv_min, mesh->vertices[3].uv);
    }

    return ECS_OK;
}

