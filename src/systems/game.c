/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : game
 * @created     : Mardi jan 07, 2025 15:09:07 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/vec3.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "systems.h"
#include "utils/vector.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_enemy_init(ecs_entity_t *it, int count, void *args)
{
    srand(time(NULL));

    for (int i = 0; i < count; ++i)
    {
        transform_t *t;
        rigidbody_t *rb;
        ecs_get_component(it[i], transform_t, &t);
        ecs_get_component(it[i], rigidbody_t, &rb);

        int yoffset = rand() % 8;
        t->position[0] = 8;
        t->position[1] = 14 + yoffset;

        rb->velocity[0] = 1.0 * (rand() % 10) / 5.0;
    }

    return ECS_OK;
}

ecs_err_t system_enemy_update(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        transform_t *t;
        rigidbody_t *rb;
        ecs_get_component(it[i], transform_t, &t);
        ecs_get_component(it[i], rigidbody_t, &rb);

        if (t->position[0] > 16)
        {
            t->position[0] = 8;
        }
    }

    return ECS_OK;
}

ecs_err_t system_player_init(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        transform_t *t;
        rigidbody_t *rb;
        ecs_get_component(it[i], transform_t, &t);
        ecs_get_component(it[i], rigidbody_t, &rb);
    }

    return ECS_OK;
}

ecs_err_t system_player_update(ecs_entity_t *it, int count, void *args)
{
    void **player_args = (void **)args;

    bool *gameover = (bool *)player_args[0];
    float dt = *((float *)player_args[1]);

    for (int i = 0; i < count; ++i)
    {
        transform_t *t;
        rigidbody_t *rb;
        rect_collider_t *col;
        camera_t *cam;
        ecs_get_component(it[i], transform_t, &t);
        ecs_get_component(it[i], rigidbody_t, &rb);
        ecs_get_component(it[i], rect_collider_t, &col);
        ecs_get_component(it[i], camera_t, &cam);

        if (*gameover)
        {
                controller_t *ctrl;
                audio_t *audio;
                ecs_get_component(it[i], controller_t, &ctrl);
                ecs_get_component(it[i], audio_t, &audio);

                ctrl->run_speed = 0;
                ctrl->walk_speed = 0;
                cam->color_filter_strength += dt * 0.7f;
                if (cam->color_filter_strength > 1) cam->color_filter_strength = 1;
                cam->detach = true;
                audio->volume -= dt * 0.2;
                if (audio->volume < 0) 
                {
                    audio->volume = 0;
                    audio->playing = false;
                }
        }

        /* printf("player pos (%.1f, %.1f)\n", t->position[0], t->position[1]); */
        /* printf("entity col count : %i\n", col->entities.size); */

        if (t->position[1] > 10 && t->position[1] < 14)
        {
            cam->zoom = 1 / (t->position[1] - 10 + 1) * 20;
        }
        if (t->position[1] > 22)
        {
            t->position[1] = 14;
        }

        for (int j = 0; j < col->entities.size; ++j)
        {
            ecs_entity_t col_entity;
            vector_get_copy(&col->entities, j, &col_entity);
            if (ecs_entity_has_component(col_entity, enemy_t))
            {
                *gameover = true;
            }
        }
    }

    return ECS_OK;
}
