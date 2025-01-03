/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : mesh
 * @created     : Vendredi jan 03, 2025 20:42:44 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/vec3.h"
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
typedef struct
{
    vec3 position;
    vec2 uv;
} vertex_t;

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static const vertex_t vertices[] =
{
    {   
        .position={ 0.5f, 0.5f, 0.0f }, // top-right
        .uv={ 1, 1 } 
    },
    {   
        .position={ 0.5f, -0.5f, 0.0f }, // bottom-right
        .uv={ 1, 0 } 
    },
    {   
        .position={ -0.5f,  0.5f, 0.0f }, // top-left
        .uv={ 0, 1 } 
    },
    {   
        .position={ -0.5f, -0.5f, 0.0f }, // bottom-left
        .uv={ 0, 0 }
    },
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

    for (int i = 0; i < count; ++i)
    {
        mesh_t *mesh;
        ecs_get_component(it[i], mesh_t, &mesh);

        glGenVertexArrays(1, &mesh->VAO);
        glGenBuffers(1, &mesh->VBO);
        glGenBuffers(1, &mesh->EBO);

        glBindVertexArray(mesh->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

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

ecs_err_t system_mesh_update(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        mesh_t *mesh;
        transform_t *transform;
        ecs_get_component(it[i], mesh_t, &mesh);
        ecs_get_component(it[i], transform_t, &transform);

        vertex_t *new_vertices = malloc(sizeof(vertices));
        memcpy(new_vertices, vertices, sizeof(vertices));
        for (int i = 0; i < 4; ++i) 
        {
            glm_vec3_add((float *)vertices[i].position, transform->position, new_vertices[i].position);

        }

        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), new_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        free(new_vertices);


        glBindVertexArray(mesh->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    return ECS_OK;
}

