/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : physics
 * @created     : Vendredi jan 03, 2025 22:15:47 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/vec3.h"
#include "cglm/io.h"
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
static bool check_collision(transform_t *t1, collider_t *c1, transform_t *t2, collider_t *c2);
static bool check_all_collision(transform_t *t1, collider_t *c1, int ind, ecs_entity_t *it, int count);

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

ecs_err_t system_collider_init(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        transform_t *transform;
        collider_t *collider;
        ecs_get_component(it[i], transform_t, &transform);
        ecs_get_component(it[i], collider_t, &collider);

        if (collider->shape == 0)
        {
            collider->shape = COLLIDER_SQUARE;
        }

        if (glm_vec3_eqv(collider->size, (vec3){ 0 }))
        {
            glm_vec3_copy(transform->scale, collider->size);
        }
    }

    return ECS_OK;       
}

inline bool check_collision(transform_t *t1, collider_t *c1, transform_t *t2, collider_t *c2)
{
    // Calculate the AABB min and max for the first collider
    float min1[3] = {
        t1->position[0] + c1->offset[0] - c1->size[0] / 2.0f,
        t1->position[1] + c1->offset[1] - c1->size[1] / 2.0f,
        t1->position[2] + c1->offset[2] - c1->size[2] / 2.0f
    };

    float max1[3] = {
        t1->position[0] + c1->offset[0] + c1->size[0] / 2.0f,
        t1->position[1] + c1->offset[1] + c1->size[1] / 2.0f,
        t1->position[2] + c1->offset[2] + c1->size[2] / 2.0f
    };

    // Calculate the AABB min and max for the second clider
    float min2[3] = {
        t2->position[0] + c2->offset[0] - c2->size[0] / 2.0f,
        t2->position[1] + c2->offset[1] - c2->size[1] / 2.0f,
        t2->position[2] + c2->offset[2] - c2->size[2] / 2.0f
    };

    float max2[3] = {
        t2->position[0] + c2->offset[0] + c2->size[0] / 2.0f,
        t2->position[1] + c2->offset[1] + c2->size[1] / 2.0f,
        t2->position[2] + c2->offset[2] + c2->size[2] / 2.0f
    };

    // Check for overlap on each axis
    bool overlapX = (max1[0] >= min2[0]) && (min1[0] <= max2[0]);
    bool overlapY = (max1[1] >= min2[1]) && (min1[1] <= max2[1]);
    bool overlapZ = (max1[2] >= min2[2]) && (min1[2] <= max2[2]);

    return overlapX && overlapY && overlapZ;
}

inline bool check_all_collision(transform_t *t1, collider_t *c1, int ind, ecs_entity_t *it, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (i == ind) continue;

        transform_t *ot;
        collider_t *oc;
        ecs_get_component(it[i], transform_t, &ot);
        ecs_get_component(it[i], collider_t, &oc);

        if (check_collision(t1, c1, ot, oc))
        {
            return true;
        }
    }      

    return false;
}

ecs_err_t system_collider_update(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        collider_t *collider;
        ecs_get_component(it[i], collider_t, &collider);
        if (!collider->is_trigger) 
        {
            continue;
        }

        transform_t *transform;
        ecs_get_component(it[i], transform_t, &transform);
        rigidbody_t *rb;
        ecs_get_component(it[i], rigidbody_t, &rb);


        if (check_all_collision(transform, collider, i, it, count))
        {
            vec3 dir;
            glm_vec3_sub(collider->impact_position, transform->position, dir);
            glm_vec3_normalize(dir);

            /* vec3 new_velocity; */
            /* glm_vec3_scale(dir, glm_vec3_dot(rb->velocity, dir), new_velocity); */
            /* glm_vec3_sub(rb->velocity, new_velocity, new_velocity); */
            /* glm_vec3_copy(new_velocity, rb->velocity); */

            glm_vec3_copy(collider->impact_position, transform->position);
            /* printf("touching\n"); */
        }
        else
        {
            glm_vec3_copy(transform->position, collider->impact_position);
        }
    }

    return ECS_OK;       
}
