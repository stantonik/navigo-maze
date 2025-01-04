/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : physics
 * @created     : Vendredi jan 03, 2025 22:15:47 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/vec3.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
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

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_mouvement_init(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        transform_t *transform;
        ecs_get_component(it[i], transform_t, &transform);

        if (glm_vec3_eqv(transform->scale, (vec3){ 0 }))
        {
            glm_vec3_one(transform->scale);
        }
    }

    return ECS_OK;
}

ecs_err_t system_mouvement_update(ecs_entity_t *it, int count, void *args)
{
    float dt = ((float *)args)[0];

    for (int i = 0; i < count; ++i)
    {
        transform_t *transform;
        rigidbody_t *rb;
        ecs_get_component(it[i], transform_t, &transform);
        ecs_get_component(it[i], rigidbody_t, &rb);

        if (rb->gravity)
        {
            rb->velocity[1] -= 9.81 * dt;
        }

        float normal_force = rb->mass * 9.81f;
        float friction_force = 0.05f * normal_force;

        float friction_direction_x = (rb->velocity[0] > 0) ? -1.0f : 1.0f;
        float friction_direction_y = (rb->velocity[1] > 0) ? -1.0f : 1.0f;

        if (fabs(rb->velocity[0]) > 0) 
        {
            rb->velocity[0] += friction_direction_x * friction_force * dt;
        } 

        if (fabs(rb->velocity[1]) > 0)
        {
            rb->velocity[1] += friction_direction_y * friction_force * dt;
        }

        if (friction_direction_x < 0)
        {
            if (rb->velocity[0] < 0)
            {
                rb->velocity[0] = 0;
            }
        }
        else
        {
            if (rb->velocity[0] > 0)
            {
                rb->velocity[0] = 0;
            }
        }

        if (friction_direction_y < 0)
        {
            if (rb->velocity[1] < 0)
            {
                rb->velocity[1] = 0;
            }
        }
        else
        {
            if (rb->velocity[1] > 0)
            {
                rb->velocity[1] = 0;
            }
        }

        vec3 dl;
        glm_vec3_scale(rb->velocity, dt, dl);
        glm_vec3_add(transform->position, dl, transform->position);
    }

    return ECS_OK;
}


