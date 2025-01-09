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
// Macros
//------------------------------------------------------------------------------
#define LOG_DIR "./res/logs"
#define LOG_PATH LOG_DIR"/score.txt"

#define ROAD_CAM_ZOOM 5
#define ROAD_Y_TRANS_BEGIN 10
#define ROAD_Y_TRANS_END 12
#define ROAD_Y_BEGIN 16
#define ROAD_Y_END 22
#define ROAD_X_MIN 12
#define ROAD_X_MAX 16
#define BACKWARD_FREEDOM 1

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_enemy_init(ecs_entity_t *it, int count, void *args[])
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

ecs_err_t system_enemy_update(ecs_entity_t *it, int count, void *args[])
{
    ecs_entity_t ep = *(ecs_entity_t *)args[0];
    player_t *player;
    ecs_get_component(ep, player_t, &player);

    for (int i = 0; i < count; ++i)
    {
        transform_t *t;
        rigidbody_t *rb;
        ecs_get_component(it[i], transform_t, &t);
        ecs_get_component(it[i], rigidbody_t, &rb);

        if (player->tped)
        {
            int xview = ROAD_CAM_ZOOM / 2;
            int dx = ROAD_Y_END - ROAD_Y_BEGIN;
            if (t->position[1] <= ROAD_Y_END + xview && t->position[1] >= ROAD_Y_END - xview)
            {
                t->position[1] -= dx;
            }
            else if (t->position[1] <= ROAD_Y_BEGIN + xview && t->position[1] >= ROAD_Y_BEGIN - xview)
            {
                t->position[1] += dx;
            }
        }

        if (t->position[0] > 16)
        {
            t->position[0] = 8;
        }
    }

    if (player->tped)
    {
        player->tped = false;
    }

    return ECS_OK;
}

ecs_err_t system_player_init(ecs_entity_t *it, int count, void *args[])
{
    for (int i = 0; i < count; ++i)
    {
        player_t *player;
        ecs_get_component(it[i], player_t, &player);

        // Check if the log file exists
        FILE *file = fopen(LOG_PATH, "r");
        if (file == NULL) 
        {
            file = fopen(LOG_PATH, "w");
            if (file == NULL)
            {
                printf("Error creating log file at %s\n", LOG_PATH);
                return ECS_ERR;
            }
            player->best_score = 0;
            fprintf(file, "%.1f\n", 0.0f);
            printf("File created: %s\n", LOG_PATH);
            fclose(file);
        }
        else 
        {
            char line[64];
            if (fgets(line, sizeof(line), file) != NULL) 
            {
                player->best_score = atof(line);
            }
            else 
            {
                player->best_score = 0;
            }
            printf("Read '%.1f' as player bestscore from: %s\n", player->best_score, LOG_PATH);
            fclose(file);
        }

    }

    return ECS_OK;
}

ecs_err_t system_player_update(ecs_entity_t *it, int count, void *args[])
{
    bool *gameover = (bool *)args[0];
    float dt = *(float *)args[1];

    for (int i = 0; i < count; ++i)
    {
        transform_t *t;
        rigidbody_t *rb;
        rect_collider_t *col;
        player_t *player;

        camera_t *cam;
        transform_t *cam_tranform;

        text_t *score_txt;
        transform_t *score_transform;

        ecs_get_component(it[i], transform_t, &t);
        ecs_get_component(it[i], rigidbody_t, &rb);
        ecs_get_component(it[i], rect_collider_t, &col);
        ecs_get_component(it[i], player_t, &player);

        ecs_get_component(player->ecamera, camera_t, &cam);
        ecs_get_component(player->ecamera, transform_t, &cam_tranform);

        ecs_get_component(player->escore, text_t, &score_txt);
        ecs_get_component(player->escore, transform_t, &score_transform);

        if (*gameover)
        {
            audio_t *audio;
            ecs_get_component(it[i], audio_t, &audio);

            cam->color_filter_strength += dt * 0.7f;
            if (cam->color_filter_strength > 1) cam->color_filter_strength = 1;
            audio->volume -= dt * 0.2;
            if (audio->volume < 0) 
            {
                audio->volume = 0;
                audio->playing = false;
            }
        }
        else
        {
            /* printf("player pos (%.1f, %.1f)\n", t->position[0], t->position[1]); */

            // Constrain player on the Y axe
            if (t->position[1] > ROAD_Y_END)
            {
                float previous_player_pos_y = t->position[1];

                t->position[1] = ROAD_Y_BEGIN;
                player->tped = true;

                if (previous_player_pos_y != ROAD_Y_BEGIN)
                {
                    float cam_offset = cam_tranform->position[1] - previous_player_pos_y;
                    cam_tranform->position[1] = ROAD_Y_BEGIN + cam_offset;

                    float score_txt_offset = score_transform->position[1] - previous_player_pos_y;
                    score_transform->position[1] = ROAD_Y_BEGIN + score_txt_offset;
                }
            }
            vec3 cam_target_pos;
            glm_vec3_copy(t->position, cam_target_pos);

            // Smooth zoom entry
            float target_zoom = 20 + (ROAD_CAM_ZOOM - 20) * (t->position[1] - ROAD_Y_TRANS_BEGIN) / (ROAD_Y_TRANS_END - ROAD_Y_TRANS_BEGIN);
            target_zoom = glm_clamp(target_zoom, ROAD_CAM_ZOOM, 20);
            float zoom_factor = 1.0f - expf(-5 * dt);
            cam->zoom = glm_lerp(cam->zoom, target_zoom, zoom_factor);

            // Smooth score appearing
            float target_alpha = 0.2 + (1 - 0.2) * (t->position[1] - ROAD_Y_TRANS_BEGIN) / (ROAD_Y_TRANS_END - ROAD_Y_TRANS_BEGIN);
            target_alpha = glm_clamp(target_alpha, 0.0f, 0.8f);
            float alpha_factor = 1.0f - expf(-8 * dt);
            score_txt->color[3] = glm_lerp(score_txt->color[3], target_alpha, alpha_factor);


            if (t->position[1] <= ROAD_Y_END && t->position[1] >= ROAD_Y_TRANS_END)
            {
                score_txt->text[0] = '\0';
                snprintf(score_txt->text, 50, "%.1f", player->score);

                // Forbid to go too backward
                if (t->position[1] < cam_tranform->position[1] - BACKWARD_FREEDOM)
                {
                    if (rb->velocity[1] < 0)
                    {
                        rb->velocity[1] = 0;
                    }
                }

                // Move camera on Y only if we go upward
                if (cam_target_pos[1] < cam_tranform->position[1] && glm_vec3_distance(cam_target_pos, t->position) < 3)
                {
                    cam_target_pos[1] = cam_tranform->position[1];
                } 
                else
                {
                    // Increment score by the distance traveled
                    player->score += rb->velocity[1] * dt;
                }
            }
            if (t->position[1] <= ROAD_Y_END && t->position[1] >= ROAD_Y_TRANS_BEGIN)
            {
                // Constrain player on the road on the X axe
                if (t->position[0] < ROAD_X_MIN)
                {
                    if (rb->velocity[0] < 0)
                    {
                        rb->velocity[0] = 0;
                    }
                }
                else if (t->position[0] > ROAD_X_MAX - 1)
                {
                    if (rb->velocity[0] > 0)
                    {
                        rb->velocity[0] = 0;
                    }
                }
            }

            float factor = 1.0f - expf(-player->cam_lerp_speed * dt);
            glm_vec3_lerp(cam_tranform->position, cam_target_pos, factor, cam_tranform->position);

            factor = 1.0f - expf(-10 * dt);
            vec3 score_target_pos;
            glm_vec3_add(t->position, (vec3){ -0.5, 1 }, score_target_pos);
            glm_vec3_lerp(score_transform->position, score_target_pos, factor, score_transform->position);


            // Check collision with bicycle and thus gameover
            for (int j = 0; j < col->entities.size; ++j)
            {
                ecs_entity_t col_entity;
                vector_get_copy(&col->entities, j, &col_entity);
                if (ecs_entity_has_component(col_entity, enemy_t) && !*gameover)
                {
                    bool isbest = player->score > player->best_score;
                    if (isbest)
                    {
                        player->best_score = player->score;
                    }
                    score_transform->position[2] = -0.9;
                    score_txt->text[0] = '\0';
                    snprintf(score_txt->text, 100, "score:%.1f best:%.1f", player->score, player->best_score);

                    // Save the bestscore
                    if (isbest)
                    {
                        printf("Writing to the log file...\n");
                        /* FILE *file = fopen(LOG_PATH, "w"); */
                        /* if (file == NULL) */
                        /* { */
                        /*     printf("Error saving score\n"); */
                        /* } */
                        /* else */
                        /* { */
                        /*     printf("New bestscore saved (%.1f)\n", player->best_score); */
                        /*     fprintf(file, "%.1f\n", player->best_score); */
                        /*     fclose(file); */
                        /* } */
                    }

                    player->score = 0;
                    *gameover = true;
                }
            }
        }   
    }

    return ECS_OK;
}

ecs_err_t system_player_restart(ecs_entity_t *it, int count, void *args[])
{
    bool *gameover = (bool *)args[0];
    bool *restart = (bool *)args[1];

    if (!*restart) 
    {
        return ECS_OK;
    }

    for (int i = 0; i < count; ++i)
    {
        camera_t *cam;
        transform_t *cam_t;

        text_t *score_txt;
        transform_t *score_t;

        transform_t *t;
        audio_t *audio;
        player_t *player;
        ecs_get_component(it[i], transform_t, &t);
        ecs_get_component(it[i], audio_t, &audio);
        ecs_get_component(it[i], player_t, &player);

        ecs_get_component(player->ecamera, camera_t, &cam);
        ecs_get_component(player->ecamera, transform_t, &cam_t);

        ecs_get_component(player->escore, text_t, &score_txt);
        ecs_get_component(player->escore, transform_t, &score_t);

        glm_vec3_copy((vec3){ ROAD_X_MIN + 1, ROAD_Y_TRANS_BEGIN - 2 }, t->position);
        cam->color_filter_strength = 0;
        cam->zoom = 20;
        player->cam_lerp_speed = 4;
        audio->volume = 0.5;
        audio->playing = true;
        audio->restart = true;
        score_txt->text[0] = '\0';
        score_t->position[2] = 0;
        if (glm_vec3_distance(t->position, cam_t->position) < 1)
        {
            player->cam_lerp_speed = 5;
            *restart = false;
        }
    }

    *gameover = false;

    return ECS_OK;
}
