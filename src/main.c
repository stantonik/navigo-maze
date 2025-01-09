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

#include <stdbool.h>
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
// Static Variables
//------------------------------------------------------------------------------
static float dt = 0;
static bool gameover = false;
static bool restart = false;

ecs_entity_t player;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static double get_time();
static void init_game();

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
int main(void)
{
    gfx_init_opengl();
    shader_init();
    ecs_init();

    init_game();

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

        if (fps_timer >= 1.0)
        {
            fps = frame_count / fps_timer;
            printf("\rFPS: %.0f", fps);
            fflush(stdout);
            frame_count = 0;
            fps_timer = 0.0;
        }

        glClearColor(0.11f, 0.11f, 0.10f, 0.0f);
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

inline void init_game()
{
    // Scene creation
    ecs_scene_t scene;
    ecs_create_scene(&scene);
    ecs_bind_scene(scene);

    // Components registration
    ecs_register_component(transform_t);
    ecs_register_component(rigidbody_t);
    ecs_register_component(rect_collider_t);
    ecs_register_component(sprite_t);
    ecs_register_component(controller_t);
    ecs_register_component(camera_t);
    ecs_register_component(text_t);
    ecs_register_component(enemy_t);
    ecs_register_component(player_t);
    ecs_register_component(audio_t);

    // Systems registration
    ecs_signature_t signature;
    ecs_create_signature(&signature, sprite_t);
    ecs_register_system(system_texture_init, signature, ECS_SYSTEM_ON_INIT);

    ecs_create_signature(&signature, audio_t);
    ecs_register_system(system_audio_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_audio_update, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, transform_t, sprite_t);
    ecs_register_system(system_mesh_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_mesh_update, signature, ECS_SYSTEM_ON_UPDATE);
    ecs_create_signature(&signature, sprite_t);
    ecs_register_system(system_mesh_draw, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, transform_t, camera_t);
    ecs_register_system(system_camera_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_camera_update, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, transform_t, rigidbody_t, controller_t, rect_collider_t, player_t);
    ecs_register_system(system_player_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_player_update, signature, ECS_SYSTEM_ON_UPDATE);
    ecs_set_system_parameters(system_player_update, 2, (void *[]){ &gameover, &dt });
    ecs_register_system(system_player_restart, signature, ECS_SYSTEM_ON_UPDATE);
    ecs_set_system_parameters(system_player_restart, 2, (void *[]){ &gameover, &restart });

    ecs_create_signature(&signature, transform_t, rigidbody_t, enemy_t);
    ecs_register_system(system_enemy_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_enemy_update, signature, ECS_SYSTEM_ON_UPDATE);
    ecs_set_system_parameters(system_enemy_update, 1, (void *[]){ &player });

    ecs_create_signature(&signature, transform_t);
    ecs_register_system(system_mouvement_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_create_signature(&signature, transform_t, rigidbody_t);
    ecs_register_system(system_mouvement_update, signature, ECS_SYSTEM_ON_UPDATE);
    ecs_set_system_parameters(system_mouvement_update, 1, (void *[]){ &dt });

    ecs_create_signature(&signature, controller_t);
    ecs_register_system(system_controller_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_controller_update, signature, ECS_SYSTEM_ON_UPDATE);
    ecs_set_system_parameters(system_controller_update, 2, (void *[]){ &gameover, &restart });

    ecs_create_signature(&signature, transform_t, rigidbody_t, rect_collider_t);
    ecs_register_system(system_collider_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_collider_update, signature, ECS_SYSTEM_ON_UPDATE);

    ecs_create_signature(&signature, transform_t, text_t);
    ecs_register_system(system_text_init, signature, ECS_SYSTEM_ON_INIT);
    ecs_register_system(system_text_update, signature, ECS_SYSTEM_ON_UPDATE);

    // Entities creation
    ecs_entity_t camera;
    ecs_create_entity(&camera);
    ecs_add_component(camera, transform_t, NULL);
    ecs_add_component(camera, camera_t, &((camera_t){ .zoom=20 }));

    // Texts
    ecs_entity_t text;
    ecs_create_entity(&text);
    ecs_add_component(text, transform_t, &((transform_t){ .position={ -7, -6, -0.9 } }));
    ecs_add_component(text, text_t, &((text_t){ 
                .text="You woke up lost from a French soiree " \
                "with the only memory of an important rendez-vous. "\
                "You have to hurry up but you have lost your Navigo "\
                "so take a walk and enjoy the views!",
                .color={ 1, 1, 1, 0.8 }, .size=0.5f, .camera=camera, .max_width=5}));

    ecs_entity_t score;
    ecs_create_entity(&score);
    ecs_add_component(score, transform_t, NULL);
    ecs_add_component(score, text_t, &((text_t){ .color={ 0.1, 0.1, 0.2, 0.0 }, .text="0.0", .size=0.3f, .camera=camera, .max_width=1 }));

    ecs_create_entity(&player);
    ecs_add_component(player, transform_t, &((transform_t){ .scale={ 0.8, 0.8, 1 } }));
    ecs_add_component(player, rigidbody_t, &((rigidbody_t){ .mass=70, .friction=0.001f }));
    ecs_add_component(player, rect_collider_t, &((rect_collider_t){ .is_trigger=true, .size={ 0.6, 0.6 } }));
    ecs_add_component(player, controller_t, &((controller_t){ .walk_speed=3 }));
    ecs_add_component(player, sprite_t, &((sprite_t){ .texture_name="player" }));
    ecs_add_component(player, audio_t, &((audio_t){ .name="bg", .volume=0.5, .loop=true, .playing=true }));
    ecs_add_component(player, player_t, &((player_t){ .ecamera=camera, .escore=score, .cam_lerp_speed=5 }));

    // Velo enemies
    ecs_entity_t enemies[10];
    for (int i = 0; i < sizeof(enemies) / sizeof(enemies[0]); ++i)
    {
        ecs_entity_t enemy;
        ecs_create_entity(&enemy);
        ecs_add_component(enemy, transform_t, &((transform_t){ .position={ 12, 20 }, .scale={ 1.2, 1.2, 1 } }));
        ecs_add_component(enemy, rigidbody_t, &((rigidbody_t){ .mass=70 }));
        ecs_add_component(enemy, rect_collider_t, &((rect_collider_t){ .size={ 1, 1 } }));
        ecs_add_component(enemy, sprite_t, &((sprite_t){ .texture_name="enemy_velo" }));
        ecs_add_component(enemy, enemy_t, NULL);

        enemies[i] = enemy;
    }

    // Map generation
    map_t map = map_get(MAP_ZEBRACROSSING);
    vec2 offset = { 5, 10 };

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

        sprite_t tex = { .texture_name="city/tile_" };
        char cid[5] = "";
        sprintf(cid, "%04d", tile.id);
        strcat(tex.texture_name, cid);

        ecs_add_component(etile, transform_t, &((transform_t){ .position={ tile.x - map.width / 2.f + offset[0], map.height / 2.f - tile.y + offset[1], 0.9 } }));
        ecs_add_component(etile, sprite_t, &tex);

        if (tile.is_border)
        {
            ecs_add_component(etile, rigidbody_t, NULL);
            ecs_add_component(etile, rect_collider_t, NULL);
        }
    }
    map_free(map);
}

inline double get_time() 
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
