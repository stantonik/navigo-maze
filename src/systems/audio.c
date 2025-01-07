/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : audio
 * @created     : Mardi jan 07, 2025 23:05:56 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "systems.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define SOUND_DIR "./res/sounds"
#define SOUND_EXT ".mp3"

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static ma_engine engine;

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_audio_init(ecs_entity_t *it, int count, void *args)
{
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) 
    {
        return ECS_ERR;
    }

    return ECS_OK;
}

ecs_err_t system_audio_update(ecs_entity_t *it, int count, void *args)
{
    for (int i = 0; i < count; ++i)
    {
        audio_t *audio;
        ecs_get_component(it[i], audio_t, &audio);

        if (!audio->init)
        {
            char path[64] = SOUND_DIR"/";
            strcat(path, audio->name);
            strcat(path, SOUND_EXT);
            ma_result res = ma_sound_init_from_file(&engine, path, MA_SOUND_FLAG_STREAM, NULL, NULL, &audio->sound);
            if (res == MA_SUCCESS)
            {
                audio->init = true;
            }
        }

        if (audio->playing && audio->init)
        {
            ma_sound_set_volume(&audio->sound, audio->volume);
            if (!ma_sound_is_playing(&audio->sound))
            {
                ma_sound_start(&audio->sound);
            }
            else
            {
                if (!audio->loop)
                {
                    ma_sound_stop(&audio->sound);
                    audio->playing = false;
                }
            }
        }
    }

    return ECS_OK;
}
