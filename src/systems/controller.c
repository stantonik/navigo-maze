/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : controller
 * @created     : Vendredi jan 03, 2025 21:50:25 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/vec3.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gfx.h"
#include "systems.h"

#include <math.h>
#include <stdbool.h>

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
ecs_err_t system_controller_init(ecs_entity_t *it, int count, void *args[])
{
    if (!glfwInit())
    {
        return ECS_ERR;
    }

    glfwSetKeyCallback(gfx_get_window(), key_callback);

    return ECS_OK;
}

ecs_err_t system_controller_update(ecs_entity_t *it, int count, void *args[])
{
    GLFWwindow *window = gfx_get_window();
    itcpy = it;
    countcpy = count;

    bool gameover = *(bool *)args[0];
    bool *restart = (bool *)args[1];

    for (int i = 0; i < count; ++i)
    {
        controller_t *ctrl;
        rigidbody_t *rb;
        ecs_get_component(it[i], rigidbody_t, &rb);
        ecs_get_component(it[i], controller_t, &ctrl);

        if (gameover == false)
        {
            int a_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
            int d_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
            int w_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
            int s_pressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

            vec3 velocity = { 0 };

            if (a_pressed && d_pressed)
            {
                velocity[0] = 0;
            }
            else if (a_pressed)
            {
                velocity[0] = -1;
            }
            else if (d_pressed)
            {
                velocity[0] = 1;
            }

            if (w_pressed && s_pressed)
            {
                velocity[1] = 0;
            }
            else if (w_pressed)
            {
                velocity[1] = 1;
            }
            else if (s_pressed)
            {
                velocity[1] = -1;
            }

            glm_vec3_normalize(velocity);
            glm_vec3_scale(velocity, ctrl->walk_speed, velocity);
            glm_vec3_copy(velocity, rb->velocity);
        } 
        else
        {
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                *restart = true;
            }
        }
    }

    return ECS_OK;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /* for (int i = 0; i < countcpy; ++i) */
    /* { */
    /*     controller_t *ctrl; */
    /*     rigidbody_t *rb; */
    /*     ecs_get_component(itcpy[i], rigidbody_t, &rb); */
    /*     ecs_get_component(itcpy[i], controller_t, &ctrl); */

    /*     if (key == GLFW_KEY_SPACE) */
    /*     { */
    /*         rb->velocity[1] = sqrt(2 * 9.81 * ctrl->jump_height); */
    /*     } */
    /* } */

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


