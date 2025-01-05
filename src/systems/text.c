/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : text
 * @created     : Dimanche jan 05, 2025 00:09:16 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/io.h"
#include "cglm/types.h"
#include "GL/glew.h"
#include "cglm/vec2.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "shader.h"
#include "stb_image.h"
#include "systems.h"

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

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
GLuint bitmap_tex;

GLuint VAO, VBO, EBO;

static const unsigned int indices[] = 
{
    0, 1, 2,
    2, 1, 3 
};

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static void render_text(const char *text, float x, float y, float z, float scale, vec3 color);
static void get_char_uv(char c, vec2 uvmin, vec2 uvmax);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_text_init(ecs_entity_t *it, int count, void *args)
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
    glUniform1i(glGetUniformLocation(shader_get_program(SHADER_TEXT), "font_bitmap"), 1);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * 4, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, uv));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return ECS_OK;
}

ecs_err_t system_text_update(ecs_entity_t *it, int count, void *args)
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

        render_text(text->text, transform->position[0], transform->position[1], transform->position[2], text->size, text->color);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return ECS_OK;
}

inline void render_text(const char *text, float x, float y, float z, float scale, vec3 color)
{
    shader_use(SHADER_TEXT);
    glUniform3fv(glGetUniformLocation(shader_get_program(SHADER_TEXT), "textColor"), 1, (float *)color);

    int i = 0;
    while (text[i] != '\0') 
    {
        char c = text[i];
        if (c < BITMAP_ASCII_BEGIN || c > BITMAP_ASCII_END)
        {
            c = '?';
        }

        vec2 uvmin, uvmax;
        get_char_uv(c, uvmin, uvmax);

        float xpos = x + (BITMAP_TILE_WIDTH * i * 0.8f) * scale;
        float ypos = y - BITMAP_TILE_HEIGHT * scale;

        float w = BITMAP_TILE_WIDTH * scale;
        float h = BITMAP_TILE_HEIGHT * scale;

        vertex_t vertices[4] = { { { xpos + w, ypos + h, z } }, { { xpos + w, ypos, z } }, { { xpos, ypos + h, z } }, { { xpos, ypos, z } } };
        glm_vec2_copy(uvmax, vertices[0].uv);
        glm_vec2_copy((vec2){ uvmax[0], uvmin[1] }, vertices[1].uv);
        glm_vec2_copy((vec2){ uvmin[0], uvmax[1] }, vertices[2].uv);
        glm_vec2_copy(uvmin, vertices[3].uv);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
