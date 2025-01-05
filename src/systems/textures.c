/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : textures
 * @created     : Vendredi jan 03, 2025 22:38:04 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "cglm/vec2.h"
#include "components.h"
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "shader.h"
#include "stb_image.h"
#include "stb_rect_pack.h"
#include "systems.h"
#include "textures.h"
#include "utils/stoi_map.h"
#include "utils/vector.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define TEXTURE_DIR "./res/textures"
#define TEXTURE_FORMAT ".png"

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct 
{
    int width, height;
    int x, y;
    int nchannels;
    float umin, vmin, umax, vmax;
    char path[64];
} texture_info_t;

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static stoi_map_t name_to_texture_map = {  };
static vector_t textures = {  };
GLuint texture_id;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static ecs_err_t fill_atlas(unsigned char *atlas_data, size_t atlas_width);
static ecs_err_t get_atlas_size(int *atlas_width, int *atlas_height);
static void set_uv_coordinates(int atlas_width, int atlas_height);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t system_texture_init(ecs_entity_t *it, int count, void *args)
{
    stoi_map_init(&name_to_texture_map);
    vector_init(&textures, sizeof(texture_info_t), 1);

    for (int i = 0; i < count; ++i)
    {
        texture_t *tex;
        ecs_get_component(it[i], texture_t, &tex);

        char path[64];
        strcpy(path, TEXTURE_DIR"/");
        strcat(path, tex->name);
        strcat(path, TEXTURE_FORMAT);

        int ncha;
        stbi_info(path, &tex->width, &tex->height, &ncha);

        if (!stoi_map_get(&name_to_texture_map, tex->name, NULL))
        {
            texture_info_t tex_info = { .width=tex->width, .height=tex->height, .nchannels=ncha };
            strcpy(tex_info.path, path);

            printf("Loaded texture : %s\n", path);

            stoi_map_insert(&name_to_texture_map, tex->name, textures.size);
            vector_push_back(&textures, &tex_info);
        }
    }

    if (textures.size == 0)
    {
        printf("No textures loaded\n");
        return ECS_ERR_NULL;
    }

    int atlas_width, atlas_height;
    get_atlas_size(&atlas_width, &atlas_height);
    printf("Atlas texture size is %ix%i\n", atlas_height, atlas_height);

    set_uv_coordinates(atlas_width, atlas_height);

    unsigned char *atlas_data = calloc(1, atlas_width * atlas_height * 4);
    fill_atlas(atlas_data, atlas_width);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_width, atlas_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_data);

    GLuint shader_program = shader_get_program(SHADER_WORLD);
    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "texture1"), 0);

    free(atlas_data);

    for (int i = 0; i < count; ++i)
    {
        texture_t *tex;
        ecs_get_component(it[i], texture_t, &tex);

        int ind;
        texture_info_t *tex_info;
        stoi_map_get(&name_to_texture_map, tex->name, &ind);
        vector_get(&textures, ind, (void **)&tex_info);

        glm_vec2_copy((float[]){ tex_info->umin, tex_info->vmin }, tex->uv_min);
        glm_vec2_copy((float[]){ tex_info->umax, tex_info->vmax }, tex->uv_max);
    }

    return ECS_OK;
}

ecs_err_t inline fill_atlas(unsigned char *atlas_data, size_t atlas_width)
{
    for (int i = 0; i < textures.size; i++)
    {
        texture_info_t *tex;
        vector_get(&textures, i, (void **)&tex);

        int bin;
        const int ncha = 4;
        unsigned char *data = stbi_load(tex->path, &bin, &bin, &bin, ncha);
        if (!data)
        {
            printf("Failed to load texture: %s\n", tex->path);
            continue;
        }

        for (int y = 0; y < tex->height; y++)
        {
            int flipped_y = tex->height - 1 - y;

            memcpy(
                    atlas_data + ((tex->y + y) * atlas_width + tex->x) * ncha,
                    data + (flipped_y * tex->width * ncha),
                    tex->width * ncha
                  );
        }

        stbi_image_free(data);
    }

    return ECS_OK;
}

ecs_err_t inline get_atlas_size(int *atlas_width, int *atlas_height)
{
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    printf("GL: max texture size is %i\n", max_texture_size);

    // Initial size
    *atlas_width = 1024;
    *atlas_height = 1024;

    // Prepare rectangles
    stbrp_rect *rects = malloc(textures.size * sizeof(stbrp_rect));
    if (!rects)
    {
        printf("Memory allocation for rects failed!\n");
        return ECS_ERR_MEM;
    }

    for (size_t i = 0; i < textures.size; ++i)
    {
        texture_info_t *tex;
        vector_get(&textures, i, (void **)&tex);

        rects[i].w = tex->width;
        rects[i].h = tex->height;
    }

    stbrp_node *nodes = NULL;
    int success = 0;
    while (!success)
    {
        if (nodes)
        {
            free(nodes);
        }

        // Allocate nodes based on max texture size, not atlas width
        nodes = malloc(max_texture_size * sizeof(stbrp_node));
        if (!nodes)
        {
            printf("Memory allocation for nodes failed!\n");
            free(rects);
            return ECS_ERR_MEM;
        }

        // Initialize the context for the packing algorithm
        stbrp_context context;
        stbrp_init_target(&context, *atlas_width, *atlas_height, nodes, max_texture_size);

        // Attempt to pack rectangles
        stbrp_pack_rects(&context, rects, textures.size);

        // Check if all were packed
        success = 1;
        for (size_t i = 0; i < textures.size; i++)
        {
            texture_info_t *tex;
            vector_get(&textures, i, (void **)&tex);

            tex->x = rects[i].x;
            tex->y = rects[i].y;
            if (!rects[i].was_packed)
            {
                success = 0;
                break;
            }
        }

        // If not all textures fit, increase atlas size and try again
        if (!success)
        {
            *atlas_width *= 2;
            *atlas_height *= 2;

            // Prevent exceeding the maximum allowed texture size
            if (*atlas_width > max_texture_size || *atlas_height > max_texture_size)
            {
                *atlas_width = *atlas_height = 0;
                free(rects);
                free(nodes);

                printf("Error: Textures do not fit into the maximum allowed atlas size.\n");
                return ECS_ERR_MEM;
            }
        }
    }

    free(rects);
    free(nodes);

    return ECS_OK;
}

void inline set_uv_coordinates(int atlas_width, int atlas_height)
{
    for (size_t i = 0; i < textures.size; i++)
    {
        texture_info_t *tex;
        vector_get(&textures, i, (void **)&tex);

        tex->umin = (float)tex->x / atlas_width;
        tex->vmin = (float)tex->y / atlas_height;
        tex->umax = (float)(tex->x + tex->width) / atlas_width;
        tex->vmax = (float)(tex->y + tex->height) / atlas_height;
    }
}

inline GLuint texture_get_id()
{
    return texture_id;
}
