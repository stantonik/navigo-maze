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
ecs_err_t system_mouvement_update(ecs_entity_t *it, int count, void *args)
{
    float dt = ((float *)args)[0];

    for (int i = 0; i < count; ++i)
    {
        ecs_err_t ret = ECS_OK;
        transform_t *transform;
        rigidbody_t *rb;
        ret |= ecs_get_component(it[i], transform_t, &transform);
        ret |= ecs_get_component(it[i], rigidbody_t, &rb);
        if (ret != ECS_OK)
        {
            continue;
        }

        if (rb->gravity)
        {
            rb->velocity[1] -= 9.81 * dt;
        }

        float normal_force = rb->mass * 9.81f;
        float friction_force = 0.01f * normal_force;
        float friction_direction = (rb->velocity[0] > 0) ? -1.0f : 1.0f;
        if (fabs(rb->velocity[0]) > 0) 
        {
            rb->velocity[0] += friction_direction * friction_force * dt;
        } 

        if (friction_direction < 0)
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

        vec3 dl;
        glm_vec3_scale(rb->velocity, dt, dl);
        glm_vec3_add(transform->position, dl, transform->position);
    }

    return ECS_OK;
}


