/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : main
 * @created     : Vendredi jan 03, 2025 20:10:13 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <time.h>

#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "gfx.h"
#include "systems.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static float dt = 0;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static double get_time();

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
int main(void)
{
    gfx_init_opengl();
    ecs_init();

    // Scene creation
    ecs_scene_t scene;
    ecs_create_scene(&scene);
    ecs_bind_scene(scene);

    // Components registration
    ecs_register_component(transform_t);
    ecs_register_component(rigidbody_t);
    ecs_register_component(mesh_t);
    ecs_register_component(texture_t);
    ecs_register_component(controller_t);

    // Systems registration
    ecs_signature_t signature;
    ecs_create_signature(&signature, mesh_t);
    ecs_register_system(system_mesh_init, ECS_SYSTEM_ON_INIT, signature);
    ecs_register_system(system_mesh_update, ECS_SYSTEM_ON_UPDATE, signature);

    ecs_create_signature(&signature, controller_t);
    ecs_register_system(system_controller_init, ECS_SYSTEM_ON_INIT, signature);
    ecs_register_system(system_controller_update, ECS_SYSTEM_ON_UPDATE, signature);

    ecs_create_signature(&signature, transform_t, rigidbody_t);
    ecs_register_system(system_mouvement_update, ECS_SYSTEM_ON_UPDATE, signature);
    ecs_set_system_parameters(system_mouvement_update, &dt);

    // Entities creation
    ecs_entity_t player;
    ecs_create_entity(&player);
    ecs_add_component(player, transform_t, NULL);
    ecs_add_component(player, rigidbody_t, &((rigidbody_t){ .mass=50, .gravity=false }));
    ecs_add_component(player, mesh_t, NULL);
    ecs_add_component(player, texture_t, NULL);
    ecs_add_component(player, controller_t, NULL);

    // Start displaying
    GLFWwindow *window = gfx_get_window();

    // Variables for FPS calculation
    int frame_count = 0;
    float fps_timer = 0.0f;
    float fps;

    ecs_listen_systems(ECS_SYSTEM_ON_INIT);

    // Game loop
    double previous_time = get_time();
    while(!glfwWindowShouldClose(window))
    {
        double current_time = get_time();
        dt = current_time - previous_time;
        previous_time = current_time;

        // FPS Calculation
        frame_count++;
        fps_timer += dt;

        if (fps_timer >= 1.0) // Print every 1 second
        {
            fps = frame_count / fps_timer;
            printf("\rFPS: %.0f", fps);
            fflush(stdout);
            frame_count = 0;
            fps_timer = 0.0;
        }

        glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        ecs_listen_systems(ECS_SYSTEM_ON_UPDATE);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    ecs_terminate();
    return 0;
}

inline double get_time() 
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
