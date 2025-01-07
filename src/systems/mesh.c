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
#include "shader.h"
#include "systems.h"
#include "textures.h"
#include "utils/vector.h"

#include <stdlib.h>
#include <string.h>

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

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static GLuint VAO, VBO, IBO;

static const vec3 quad_vertices[6] =
{
    { 1, 1, 0 }, // top-right
    { 0, 1, 0 }, // top-left
    { 1, 0, 0 }, // bottom-right
    { 1, 0, 0 }, // bottom-right
    { 0, 1, 0 }, // top-left
    { 0, 0, 0 }  // bottom-left
};

vector_t instances;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static void create_model_matrix(transform_t *transform, mat4 model);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_mesh_init(ecs_entity_t *it, int count, void *args)
{
    shader_use(SHADER_WORLD);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
    glEnableVertexAttribArray(ATTRIBUTE_POSITION);

    // Instance buffer
    vector_init(&instances, sizeof(instance_t), count);
    for (int i = 0; i < count; ++i)
    {
        vector_push_back(&instances, &(instance_t){});
    }
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, instances.capacity * sizeof(instance_t), NULL, GL_DYNAMIC_DRAW);

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

    return ECS_OK;
}

ecs_err_t system_mesh_draw(ecs_entity_t *it, int count, void *args)
{
    shader_use(SHADER_WORLD);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_get_id());

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances.size);

    return ECS_OK;
}

ecs_err_t system_mesh_update(ecs_entity_t *it, int count, void *args)
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
            glBufferData(GL_ARRAY_BUFFER, instances.size * instances.element_size, NULL, GL_DYNAMIC_DRAW);
        }
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size * instances.element_size, instances.data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return ECS_OK;
}

inline void create_model_matrix(transform_t *transform, mat4 model)
{
    mat4 scale_matrix;
    mat4 rotation_matrix;
    mat4 translation_matrix;
    mat4 centering_matrix;

    // Initialize matrices
    glm_mat4_identity(scale_matrix);
    glm_mat4_identity(rotation_matrix);
    glm_mat4_identity(translation_matrix);
    glm_mat4_identity(centering_matrix);

    vec2 scale;
    glm_vec3_scale(transform->scale, 1.01f, scale); // avoid gap glitchs between tiles
    glm_scale(scale_matrix, scale);

    /* glm_rotate(rotation_matrix, glm_rad(transform->rotation[0]), (vec3){1.0f, 0.0f, 0.0f}); */
    /* glm_rotate(rotation_matrix, glm_rad(transform->rotation[1]), (vec3){0.0f, 1.0f, 0.0f}); */
    /* glm_rotate(rotation_matrix, glm_rad(transform->rotation[2]), (vec3){0.0f, 0.0f, 1.0f}); */
    // Center the sprite (adjust for bottom-left reference point)
    //
    glm_translate(centering_matrix, (vec3){-scale[0] / 2.0f, -scale[1] / 2.0f, 0.0f});
    glm_translate(translation_matrix, transform->position); 

    glm_mat4_mul(centering_matrix, translation_matrix, model);
    glm_mat4_mul(model, rotation_matrix, model);
    glm_mat4_mul(model, scale_matrix, model);
}
