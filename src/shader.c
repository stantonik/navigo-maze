/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : shader
 * @created     : Samedi jan 04, 2025 23:35:44 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define SHADER_DIR "./res/shaders"

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static GLuint programs[3];

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static void load(const char *vert, const char *frag, shader_type_t type);
static inline char *read_file_to_string(const char *filename);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
inline void shader_init()
{
    load("world_vertex.glsl", "world_fragment.glsl", SHADER_WORLD);
    load("text_vertex.glsl", "text_fragment.glsl", SHADER_TEXT);
    /* load("ui_vertex.glsl", "ui_fragment.glsl", SHADER_UI); */
}

void load(const char *vert, const char *frag, shader_type_t type)
{
    GLint success;
    GLchar info_log[512];
    char path[64] = { 0 };

    // Vertex shader
    sprintf(path, "%s/%s", SHADER_DIR, vert);
    GLchar *vertex_shader_source = read_file_to_string(path);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar **)&vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    free(vertex_shader_source);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        printf("Failed to compile vertex shader: %s", info_log);
    }

    // Fragment shader
    sprintf(path, "%s/%s", SHADER_DIR, frag);
    GLchar *fragment_shader_source= read_file_to_string(path);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const GLchar **)&fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    free(fragment_shader_source);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        printf("Failed to compile fragment shader: %s", info_log);
    }

    // Create program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("Failed to build program shader: %s", info_log);
    }

    programs[type] = shader_program;

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

inline void shader_use(shader_type_t type)
{
    glUseProgram(programs[type]);
}

inline GLuint shader_get_program(shader_type_t type)
{
    return programs[type];
}

inline char *read_file_to_string(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size < 0)
    {
        fclose(file);
        return NULL;
    }

    char *buffer = malloc((file_size + 1) * sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, sizeof(char), file_size, file);
    if (read_size != file_size)
    {
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';
    fclose(file);

    return buffer;
}
