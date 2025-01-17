/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : systems
 * @created     : Vendredi jan 03, 2025 20:37:26 CET
 */

#ifndef SYSTEMS_H
#define SYSTEMS_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "ecs/ecs.h"

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
extern ecs_err_t system_mesh_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_mesh_update(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_mesh_draw(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_texture_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_texture_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_animation_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_controller_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_controller_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_camera_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_camera_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_mouvement_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_mouvement_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_collider_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_collider_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_text_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_text_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_enemy_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_enemy_update(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_player_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_player_update(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_player_restart(ecs_entity_t *it, int count, void *args[]);

extern ecs_err_t system_audio_init(ecs_entity_t *it, int count, void *args[]);
extern ecs_err_t system_audio_update(ecs_entity_t *it, int count, void *args[]);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* SYSTEMS_H */

