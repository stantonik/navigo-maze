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

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static inline void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, SCR_TITLE, NULL, NULL);
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
}

inline GLFWwindow *gfx_get_window()
{
    return window;
}

static inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


