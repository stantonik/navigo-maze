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
#include <string.h>
#include <time.h>

#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "gfx.h"
#include "maps.h"
#include "shader.h"
#include "systems.h"
#include "utils/vector.h"

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
    shader_init();
    ecs_init();

    // Scene creation
    ecs_scene_t scene;
    ecs_create_scene(&scene);
    ecs_bind_scene(scene);

    // Components registration
    ecs_register_component(transform_t);
    ecs_register_component(rigidbody_t);
    ecs_register_component(collider_t);
    ecs_register_component(mesh_t);
    ecs_register_component(texture_t);
    ecs_register_component(controller_t);
    ecs_register_component(camera_t);
    ecs_register_component(text_t);

    // Systems registration
    ecs_signature_t signature;
    ecs_create_signature(&signature, controller_t);
    ecs_register_system(system_controller_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_controller_update, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, transform_t);
    ecs_register_system(system_mouvement_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_create_signature(&signature, transform_t, rigidbody_t);
    ecs_register_system(system_mouvement_update, signature, ECS_SYSTEM_ON_UPDATE);
    ecs_set_system_parameters(system_mouvement_update, &dt);

    ecs_create_signature(&signature, transform_t, rigidbody_t, collider_t);
    ecs_register_system(system_collider_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_collider_update, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, texture_t);
    ecs_register_system(system_texture_init, signature, ECS_SYSTEM_ON_INIT);

    ecs_create_signature(&signature, mesh_t, texture_t);
    ecs_register_system(system_mesh_texture_update, signature, ECS_SYSTEM_ON_INIT);

    ecs_create_signature(&signature, transform_t, mesh_t);
    ecs_register_system(system_mesh_init, signature, ECS_SYSTEM_ON_INIT);

    ecs_create_signature(&signature, transform_t, rigidbody_t, mesh_t);
    ecs_register_system(system_mesh_update, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, mesh_t);
    ecs_register_system(system_mesh_draw, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, transform_t, camera_t);
    ecs_register_system(system_camera_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_camera_update, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, transform_t, text_t);
    ecs_register_system(system_text_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_text_update, signature, ECS_SYSTEM_ON_UPDATE);

    // Entities creation
    ecs_entity_t player;
    ecs_create_entity(&player);
    ecs_add_component(player, transform_t, &((transform_t){ .scale={ 0.8, 0.8, 1 } }));
    ecs_add_component(player, rigidbody_t, &((rigidbody_t){ .mass=70, }));
    ecs_add_component(player, collider_t, &((collider_t){ .is_trigger=true }));
    ecs_add_component(player, mesh_t, NULL);
    ecs_add_component(player, controller_t, &((controller_t){ .walk_speed=3 }));
    ecs_add_component(player, texture_t, &((texture_t){ .name="dungeon/tile_0099" }));
    ecs_add_component(player, camera_t, &((camera_t){ .zoom=20 }));

    // Decrease FPS a lot
    ecs_entity_t text;
    ecs_create_entity(&text);
    ecs_add_component(text, transform_t, &((transform_t){ .position={ 0, 0, -0.9 } }));
    ecs_add_component(text, text_t, &((text_t){ .text="Hello world!", .color={ 1, 1, 0 }, .size=0.05 }));

    // Map
    map_t map = map_get(MAP_STATION);
    vec2 offset = { 5, 5 };

    printf("Map size (%u, %u)\n", map.width, map.height);
    printf("Map tile count : %i\n", map.tiles.size);

    ecs_entity_t tiles[map.tiles.size];
    for (int i = 0; i < map.tiles.size; ++i)
    {
        tile_t tile;
        vector_get_copy(&map.tiles, i, &tile);

        ecs_entity_t etile;
        ecs_create_entity(&etile);
        tiles[i] = etile;

        texture_t tex = { .name="city/tile_" };
        char cid[5] = "";
        sprintf(cid, "%04d", tile.id);
        strcat(tex.name, cid);

        ecs_add_component(etile, transform_t, &((transform_t){ .position={ tile.x - map.width / 2.f + offset[0], map.height / 2.f - tile.y + offset[1], 0.9 } }));
        ecs_add_component(etile, mesh_t, NULL);
        ecs_add_component(etile, texture_t, &tex);

        if (tile.is_border)
        {
            ecs_add_component(etile, rigidbody_t, NULL);
            ecs_add_component(etile, collider_t, NULL);
        }
    }
    map_free(map);

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

        glClearColor(0.11f, 0.11f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
