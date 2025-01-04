/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : gfx
 * @created     : Vendredi jan 03, 2025 19:45:10 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "gfx.h"
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define SRC_TITLE "NavigoMaze"

#define SHADER_DIR "./src/shaders"

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static GLFWwindow *window = NULL;
static GLuint shader_program = 0;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static inline void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static inline char *read_file_to_string(const char *filename);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
inline void gfx_init_opengl()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, SRC_TITLE, NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create windows");
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Remove little freezes idk why
    glfwSwapInterval(GLFW_FALSE);

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW");
        exit(1);
    }

    GLint success;
    GLchar info_log[512];

    // Vertex shader
    GLchar *vertex_shader_source = read_file_to_string(SHADER_DIR"/shader.vert");
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
    GLchar *fragment_shader_source= read_file_to_string(SHADER_DIR"/shader.frag");
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
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("Failed to build program shader: %s", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

inline GLuint gfx_get_shader_program()
{
    return shader_program;
}

inline GLFWwindow *gfx_get_window()
{
    return window;
}

static inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static inline char *read_file_to_string(const char *filename)
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
