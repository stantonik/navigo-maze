/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : mesh
 * @created     : Vendredi jan 03, 2025 20:42:44 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/io.h"
#include "cglm/types.h"
#include "cglm/affine.h"
#include "cglm/mat4.h"
#include "cglm/vec3.h"
#include "cglm/vec2.h"
#include "components.h"
#include "ecs/ecs_err.h"
#include "gfx.h"
#include "mesh.h"
#include "shader.h"
#include "systems.h"
#include "textures.h"
#include "utils/vector.h"

#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static GLuint VAO, VBO, IBO;

static vector_t instances;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_mesh_init(ecs_entity_t *it, int count, void *args[])
{
    shader_use(SHADER_WORLD);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vector_init(&instances, sizeof(instance_t), count);
    for (int i = 0; i < count; ++i)
    {
        vector_push_back(&instances, &(instance_t){});
    }

    mesh_instance_init(&VAO, &VBO, &IBO, instances.capacity * sizeof(instance_t));

    return ECS_OK;
}

void mesh_instance_init(GLuint *VAO, GLuint *VBO, GLuint *IBO, size_t IBO_size)
{
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);

    // Vertex buffer
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
    glEnableVertexAttribArray(ATTRIBUTE_POSITION);

    // Instance buffer
    glGenBuffers(1, IBO);
    glBindBuffer(GL_ARRAY_BUFFER, *IBO);
    glBufferData(GL_ARRAY_BUFFER, IBO_size, NULL, GL_DYNAMIC_DRAW);

    // Model matrix attributes (4 rows)
    for (int i = 0; i < 4; ++i) {
        glVertexAttribPointer(ATTRIBUTE_MODEL_MAT_ROW0 + i, 4, GL_FLOAT, GL_FALSE, sizeof(instance_t), 
                (void *)(offsetof(instance_t, model_mat) + i * sizeof(vec4)));
        glEnableVertexAttribArray(ATTRIBUTE_MODEL_MAT_ROW0 + i);
        glVertexAttribDivisor(ATTRIBUTE_MODEL_MAT_ROW0 + i, 1);
    }

    // Color attribute
    glVertexAttribPointer(ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(instance_t), (void *)offsetof(instance_t, color));
    glEnableVertexAttribArray(ATTRIBUTE_COLOR);
    glVertexAttribDivisor(ATTRIBUTE_COLOR, 1);

    // UV offset attribute
    glVertexAttribPointer(ATTRIBUTE_UVOFFSET, 2, GL_FLOAT, GL_FALSE, sizeof(instance_t), (void *)offsetof(instance_t, uvoffset));
    glEnableVertexAttribArray(ATTRIBUTE_UVOFFSET);
    glVertexAttribDivisor(ATTRIBUTE_UVOFFSET, 1);

    // UV scale attribute
    glVertexAttribPointer(ATTRIBUTE_UVSCALE, 2, GL_FLOAT, GL_FALSE, sizeof(instance_t), (void *)offsetof(instance_t, uvscale));
    glEnableVertexAttribArray(ATTRIBUTE_UVSCALE);
    glVertexAttribDivisor(ATTRIBUTE_UVSCALE, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

ecs_err_t system_mesh_draw(ecs_entity_t *it, int count, void *args[])
{
    shader_use(SHADER_WORLD);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_get_id());

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances.size);

    return ECS_OK;
}

ecs_err_t system_mesh_update(ecs_entity_t *it, int count, void *args[])
{
    shader_use(SHADER_WORLD);
    glBindBuffer(GL_ARRAY_BUFFER, IBO);

    for (int i = 0; i < count; ++i)
    {
        sprite_t *sprite;
        ecs_get_component(it[i], sprite_t, &sprite);
        transform_t *transform;
        ecs_get_component(it[i], transform_t, &transform);

        // Update instance
        instance_t *instance;
        vector_get(&instances, i, (void **)&instance);

        transform_t tcenter = {  };
        glm_vec3_sub((vec3){ transform->scale[0] / 2.0f, transform->scale[1] / 2.0f, transform->scale[2] / 2.0f }, transform->position, tcenter.position);
        glm_vec3_copy(transform->rotation, tcenter.rotation);
        glm_vec3_copy(transform->scale, tcenter.scale);
        create_model_matrix(transform, instance->model_mat);
        glm_vec4_copy(sprite->color, instance->color);

        // UV
        glm_vec2_sub(sprite->uv_max, sprite->uv_min, instance->uvscale);
        glm_vec2_copy(sprite->uv_min, instance->uvoffset);
    }

    if (count > instances.size)
    {
        for (int i = 0; i < count - instances.size; ++i)
        {
            vector_push_back(&instances, &(instance_t){  });
        }

        glBufferData(GL_ARRAY_BUFFER, instances.size * instances.element_size, NULL, GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size * instances.element_size, instances.data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return ECS_OK;
}

