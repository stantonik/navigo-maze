/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : animations
 * @created     : Jeudi jan 09, 2025 15:47:22 CET
 */

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "components.h"
#include "ecs/ecs.h"

#include <math.h>

extern ecs_entity_t player;

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
static void title_animation(ecs_entity_t entity, float dt)
{
    text_t *txt;
    ecs_get_component(entity, text_t, &txt);

    transform_t *transform;
    ecs_get_component(entity, transform_t, &transform);

    // Animation parameters
    static float time = 0.0f;
    const float min_size = 1.8f;
    const float max_size = 2.0f;
    const float speed = 4.0f;

    time += dt * speed;

    float new_size = min_size + (max_size - min_size) * (0.5f + 0.5f * sinf(time));

    // Calculate the offset to keep the text centered
    float size_diff = new_size - txt->size;
    float offset_x = size_diff * 4.f;
    float offset_y = size_diff * 1.f;

    txt->size = new_size;

    // Adjust the transform position to keep the text centered
    transform->position[0] -= offset_x;
    transform->position[1] -= offset_y;

    transform_t *pt;
    ecs_get_component(player, transform_t, &pt);
    // Check if the player is moving
    if (fabsf(pt->position[0]) > 0.1f || fabsf(pt->position[1]) > 0.1f)
    {
        const float fade_speed = 1.f;
        txt->color[3] -= fade_speed * dt;

        if (txt->color[3] < 0.0f)
            txt->color[3] = 0.0f;
    }
    else
    {
        const float restore_speed = 1.5f;
        txt->color[3] += restore_speed * dt;

        if (txt->color[3] > 1.0f)
        {
            txt->color[3] = 1.0f;
        }
    }
}

static void subtitle_animation(ecs_entity_t entity, float dt)
{
    text_t *txt;
    ecs_get_component(entity, text_t, &txt);

    transform_t *pt;
    ecs_get_component(player, transform_t, &pt);
    // Check if the player is moving
    if (fabsf(pt->position[0]) > 0.1f || fabsf(pt->position[1]) > 0.1f)
    {
        const float fade_speed = 1.5f;
        txt->color[3] -= fade_speed * dt;

        if (txt->color[3] < 0.0f)
            txt->color[3] = 0.0f;
    }
    else
    {
        const float restore_speed = 1.5f;
        txt->color[3] += restore_speed * dt;

        if (txt->color[3] > 1.0f)
        {
            txt->color[3] = 1.0f;
        }
    }
}

static void scenario_animation(ecs_entity_t entity, float dt)
{
    text_t *txt;
    ecs_get_component(entity, text_t, &txt);

    transform_t *pt;
    ecs_get_component(player, transform_t, &pt);
    // Check if the player is moving
    if (fabsf(pt->position[0]) > 0.1f || fabsf(pt->position[1]) > 0.1f)
    {
        // Reset opacity if the player stops moving
        const float restore_speed = 0.5f;
        txt->color[3] += restore_speed * dt;

        if (txt->color[3] > 0.8f)
        {
            txt->color[3] = 0.8f;
        }
    }
    else
    {
        const float fade_speed = 0.5f;
        txt->color[3] -= fade_speed * dt;

        if (txt->color[3] < 0.0f)
        {
            txt->color[3] = 0.0f;
        }
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ANIMATIONS_H */

