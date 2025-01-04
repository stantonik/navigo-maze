/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : controller
 * @created     : Vendredi jan 03, 2025 21:50:25 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gfx.h"
#include "systems.h"

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
static ecs_entity_t *itcpy;
static int countcpy = 0;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_controller_init(ecs_entity_t *it, int count, void *args)
{
    if (!glfwInit())
    {
        return ECS_ERR;
    }

    glfwSetKeyCallback(gfx_get_window(), key_callback);

    return ECS_OK;
}

ecs_err_t system_controller_update(ecs_entity_t *it, int count, void *args)
{
    GLFWwindow *window = gfx_get_window();
    itcpy = it;
    countcpy = count;

    for (int i = 0; i < count; ++i)
    {
        rigidbody_t *rb;
        ecs_get_component(it[i], rigidbody_t, &rb);

        int a_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        int d_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

        if (a_pressed && d_pressed)
        {
            rb->velocity[0] = 0;
        }
        else if (a_pressed)
        {
            rb->velocity[0] = -1;
        }
        else if (d_pressed)
        {
            rb->velocity[0] = 1;
        }
    }

    return ECS_OK;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    for (int i = 0; i < countcpy; ++i)
    {
        rigidbody_t *rb;
        ecs_get_component(itcpy[i], rigidbody_t, &rb);

        if (key == GLFW_KEY_SPACE)
        {
            rb->velocity[1] = 3.f;
        }
    }

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


