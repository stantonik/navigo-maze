/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : text
 * @created     : Dimanche jan 05, 2025 00:09:16 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/affine.h"
#include "cglm/types.h"
#include "cglm/mat4.h"
#include "GL/glew.h"
#include "cglm/vec2.h"
#include "cglm/vec3.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "shader.h"
#include "stb_image.h"
#include "systems.h"
#include "utils/vector.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
/* #define BITMAP_PATH "./res/textures/font/test.png" */
#define BITMAP_PATH "./res/textures/font/bitmap.png"
#define BITMAP_WIDTH 128
#define BITMAP_HEIGHT 64
#define BITMAP_TILE_WIDTH 7
#define BITMAP_TILE_HEIGHT 9
#define BITMAP_HOR_COUNT 18
#define BITMAP_ASCII_BEGIN 32
#define BITMAP_ASCII_END 126

#define ATTRIBUTE_POSITION 0
#define ATTRIBUTE_MODEL_MAT_ROW0 1
#define ATTRIBUTE_MODEL_MAT_ROW1 2
#define ATTRIBUTE_MODEL_MAT_ROW2 3
#define ATTRIBUTE_MODEL_MAT_ROW3 4
#define ATTRIBUTE_UVOFFSET 6
#define ATTRIBUTE_UVSCALE 7

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct
{
    mat4 model_mat;
    vec2 uvoffset;
    vec2 uvscale;
} instance_t;

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static GLuint bitmap_tex;

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
static void render_text(const char *text, transform_t *t, float scale, vec3 color);
static void get_char_uv(char c, vec2 uvmin, vec2 uvmax);
static void create_model_matrix(transform_t *transform, mat4 model);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_text_init(ecs_entity_t *it, int count, void *args[])
{
    if (glewInit() != GLEW_OK)
    {
        return ECS_ERR;
    }

    glGenTextures(1, &bitmap_tex);
    glBindTexture(GL_TEXTURE_2D, bitmap_tex); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *data = stbi_load(BITMAP_PATH, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        printf("Loaded font bitmap : %s\n", BITMAP_PATH);
    }
    else
    {
        return ECS_ERR;
    }
    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE1);
    shader_use(SHADER_TEXT);
    glUniform1i(glGetUniformLocation(shader_get_program(SHADER_TEXT), "font_bitmap"), 1);

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
    vector_init(&instances, sizeof(instance_t), 1000);
    for (int i = 0; i < count; ++i)
    {
        vector_push_back(&instances, &(instance_t){  });
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

ecs_err_t system_text_update(ecs_entity_t *it, int count, void *args[])
{
    shader_use(SHADER_TEXT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bitmap_tex);
    glBindVertexArray(VAO);

    for (int i = 0; i < count; ++i)
    {
        transform_t *transform;
        text_t *text;
        ecs_get_component(it[i], text_t, &text);
        ecs_get_component(it[i], transform_t, &transform);

        render_text(text->text, transform, text->size, text->color);
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
    glBindVertexArray(0);

    return ECS_OK;
}

inline void render_text(const char *text, transform_t *t, float scale, vec3 color)
{
    glUniform3fv(glGetUniformLocation(shader_get_program(SHADER_TEXT), "textColor"), 1, (float *)color);

    int i = 0;
    while (text[i] != '\0') 
    {
        char c = text[i];
        if (c < BITMAP_ASCII_BEGIN || c > BITMAP_ASCII_END)
        {
            c = '?';
        }

        // Update instance
        instance_t *instance;
        vector_get(&instances, i, (void **)&instance);

        float xoffset = (BITMAP_TILE_WIDTH * i * 0.8f) * scale;
        transform_t tcpy = *t;
        tcpy.scale[0] = scale;
        tcpy.scale[1] = scale;
        glm_vec3_add((vec3){ xoffset }, tcpy.position, tcpy.position);
        create_model_matrix(&tcpy, instance->model_mat);

        // UV
        vec2 uvmin, uvmax;
        get_char_uv(c, uvmin, uvmax);
        glm_vec2_sub(uvmax, uvmin, instance->uvscale);
        glm_vec2_copy(uvmin, instance->uvoffset);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances.size);

        ++i;
    }
}

void get_char_uv(char c, vec2 uvmin, vec2 uvmax)
{
    int cind = c - BITMAP_ASCII_BEGIN;
    if (cind < 0) 
    {
        return;
    }

    int x = cind % BITMAP_HOR_COUNT;
    int y = cind / BITMAP_HOR_COUNT;

    float umin = BITMAP_TILE_WIDTH * x / (float)BITMAP_WIDTH;
    float umax = BITMAP_TILE_WIDTH * (x + 1) / (float)BITMAP_WIDTH;
    float vmin = (BITMAP_HEIGHT - BITMAP_TILE_HEIGHT * (y + 1)) / (float)BITMAP_HEIGHT;
    float vmax = (BITMAP_HEIGHT - BITMAP_TILE_HEIGHT * y) / (float)BITMAP_HEIGHT;

    glm_vec2_copy((vec2){ umin, vmin }, uvmin);
    glm_vec2_copy((vec2){ umax, vmax }, uvmax);
}

inline void create_model_matrix(transform_t *transform, mat4 model)
{
    mat4 scale_matrix;
    mat4 translation_matrix;

    glm_mat4_identity(scale_matrix);
    glm_mat4_identity(translation_matrix);
    glm_scale(scale_matrix, transform->scale);
    glm_translate(translation_matrix, transform->position); 

    glm_mat4_mul(translation_matrix, scale_matrix, model);
}
