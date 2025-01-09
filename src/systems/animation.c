/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : animation
 * @created     : Jeudi jan 09, 2025 12:28:02 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "systems.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_animation_update(ecs_entity_t *it, int count, void **args)
{
    float dt = *(float *)args[0];

    for (int i = 0; i < count; ++i)
    {
        animation_t *anim;
        ecs_get_component(it[i], animation_t, &anim);

        anim->callback(it[i], dt);
    }

    return ECS_OK;
}


