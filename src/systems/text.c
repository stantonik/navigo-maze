/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : text
 * @created     : Dimanche jan 05, 2025 00:09:16 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "camera.h"
#include "cglm/affine.h"
#include "cglm/io.h"
#include "cglm/types.h"
#include "cglm/mat4.h"
#include "GL/glew.h"
#include "cglm/vec2.h"
#include "cglm/vec3.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "gfx.h"
#include "mesh.h"
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
#define CHAR_SPACING_COEF 0.11f

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static GLuint bitmap_tex;

static GLuint VAO, VBO, IBO;

static vector_t instances;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static void populate_text_instance(int ind, const char *text, transform_t *t, float scale, vec4 color);
static void get_char_uv(char c, vec2 uvmin, vec2 uvmax);

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

    shader_use(SHADER_TEXT);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader_get_program(SHADER_TEXT), "font_bitmap"), 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Instance buffer
    int textlen = 0;
    for (int i = 0; i < count; ++i)
    {
        text_t *text;
        ecs_get_component(it[i], text_t, &text);

        textlen += strlen(text->text);    
    }

    vector_init(&instances, sizeof(instance_t), textlen);
    for (int i = 0; i < textlen; ++i)
    {
        vector_push_back(&instances, &(instance_t){  });
    }

    mesh_instance_init(&VAO, &VBO, &IBO, instances.capacity * sizeof(instance_t));

    mat4 projection;
    create_projection_matrix(SCR_WIDTH, SCR_HEIGHT, projection);
    shader_use(SHADER_TEXT);
    GLuint projection_loc = glGetUniformLocation(shader_get_program(SHADER_TEXT), "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    return ECS_OK;
}

ecs_err_t system_text_update(ecs_entity_t *it, int count, void *args[])
{
    shader_use(SHADER_TEXT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bitmap_tex);
    glBindVertexArray(VAO);

    int textlen = 0;

    for (int i = 0; i < count; ++i)
    {
        transform_t *transform;
        text_t *text;
        ecs_get_component(it[i], text_t, &text);
        ecs_get_component(it[i], transform_t, &transform);

        mat4 view;
        if (ecs_entity_has_component(text->camera, camera_t))
        {
            camera_t *cam;
            ecs_get_component(text->camera, camera_t, &cam);
            glm_mat4_dup(cam->view, view);
        }
        else
        {
            create_view_matrix(SCR_HEIGHT, &(transform_t){ .position={ SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f, 0 } }, view);
        }
        GLuint view_loc = glGetUniformLocation(shader_get_program(SHADER_TEXT), "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

        populate_text_instance(textlen, text->text, transform, text->size, text->color);

        textlen += strlen(text->text);    
    }

    glBindBuffer(GL_ARRAY_BUFFER, IBO);

    if (textlen > instances.size)
    {
        for (int i = 0; i < textlen - instances.size; ++i)
        {
            vector_push_back(&instances, &(instance_t){  });
        }

        glBufferData(GL_ARRAY_BUFFER, instances.size * instances.element_size, instances.data, GL_DYNAMIC_DRAW);
    }
    else
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, textlen * instances.element_size, instances.data);
    }

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, textlen);
    glBindVertexArray(0);

    return ECS_OK;
}

inline void populate_text_instance(int ind, const char *text, transform_t *t, float scale, vec4 color)
{
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
        vector_get(&instances, i + ind, (void **)&instance);

        float xoffset = (BITMAP_TILE_WIDTH * i * CHAR_SPACING_COEF) * scale;
        transform_t tcpy = {  };
        tcpy.scale[0] = scale;
        tcpy.scale[1] = scale;
        tcpy.scale[2] = 1;
        glm_vec3_add((vec3){ xoffset }, t->position, tcpy.position);
        create_model_matrix(&tcpy, instance->model_mat);

        // UV
        vec2 uvmin, uvmax;
        get_char_uv(c, uvmin, uvmax);
        glm_vec2_sub(uvmax, uvmin, instance->uvscale);
        glm_vec2_copy(uvmin, instance->uvoffset);
        glm_vec4_copy(color, instance->color);

        ++i;
    }
}

inline void get_char_uv(char c, vec2 uvmin, vec2 uvmax)
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
