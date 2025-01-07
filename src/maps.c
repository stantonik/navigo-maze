/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : maps
 * @created     : Samedi jan 04, 2025 15:06:08 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "jsmn.h"
#include "maps.h"
#include "utils/vector.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define MAP_DIR "./res/maps"

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
static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
static int populate_map(map_t *map, char *json);
static char *read_file_to_string(const char *filename);
static int map_is_border(int16_t *map, int width, int height, int x, int y);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
map_t map_get(map_id_t id)
{
    char path[64];

    switch (id) 
    {
        case MAP_ZEBRACROSSING:
            sprintf(path, "%s/%s", MAP_DIR, "zebracrossing.json");
            break;
        case MAP_LABYRINTH:
            sprintf(path, "%s/%s", MAP_DIR, "labyrinth.json");
            break;
        case MAP_TRAIN:
            sprintf(path, "%s/%s", MAP_DIR, "train.json");
            break;
        default:
            return (map_t){  };
    }

    char *map_data = read_file_to_string(path);
    map_t map = {  };

    if (map_data)
    {
        vector_init(&map.tiles, sizeof(tile_t), 10);

        printf("Loading map : %s\n", path);
        populate_map(&map, map_data);

        free(map_data);
    }
    else
    {
        fprintf(stderr, "Failed to load map at '%s'\n", path);
    }

    return map;
}

inline void map_free(map_t map)
{
    vector_free(&map.tiles);
}

int populate_map(map_t *map, char *json)
{
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[4000];

    jsmn_init(&p);
    r = jsmn_parse(&p, json, strlen(json), t, sizeof(t) / sizeof(t[0]));

    if (r < 0) 
    {
        printf("Map error : Failed to parse JSON: %d\n", r);
        return 1;
    }

    if (r < 1 || t[0].type != JSMN_OBJECT) 
    {
        printf("Map error : JSON Object expected\n");
        return 1;
    }

    int16_t *fullmap;

    for (i = 1; i < r; i++) 
    {
        jsmntok_t *g = &t[i];

        if (jsoneq(json, g, "mapWidth")) 
        {
            map->width = atoi(json + g[1].start);
            i++;
        } 
        else if (jsoneq(json, g, "mapHeight")) 
        {
            map->height = atoi(json + g[1].start);
            fullmap = malloc(map->height * map->width * sizeof(int16_t));
            for (int i = 0; i < map->height * map->width; ++i)
            {
                fullmap[i] = -1;
            }
            i++;
        } 
        else if (jsoneq(json, g, "layers")) 
        {
            if (jsoneq(json, &g[5], "tiles"))
            {
                for (int j = 0; j < g[6].size; ++j)
                {
                    jsmntok_t *k = &g[6 + 2 + j * 7];
                    tile_t tile;
                    tile.id = atoi(json + k[1].start);
                    tile.x = atoi(json + k[3].start);
                    tile.y = atoi(json + k[5].start);

                    fullmap[tile.x + tile.y * map->width] = tile.id;

                    vector_push_back(&map->tiles, &tile);
                }

                i+= g[6].size * 7 + 1;
            }
        } 
    }

    for (int i = 0; i < map->tiles.size; ++i)
    {
        tile_t *tile;
        vector_get(&map->tiles, i, (void **)&tile);
        tile->is_border = map_is_border(fullmap, map->width, map->height, tile->x, tile->y);
    }

    free(fullmap);

    return 0;
}

int jsoneq(const char *json, jsmntok_t *tok, const char *s) 
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start && strncmp(json + tok->start, s, tok->end - tok->start) == 0) 
    {
        return 1;
    }

    return 0;
}

inline char *read_file_to_string(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size < 0)
    {
        fclose(file);
        return NULL;
    }

    char *buffer = malloc((file_size + 1) * sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, sizeof(char), file_size, file);
    if (read_size != file_size)
    {
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';
    fclose(file);

    return buffer;
}

inline int map_is_border(int16_t *map, int width, int height, int x, int y) 
{
    if (x == 0 || y == 0 || x == width - 1 || y == height - 1)
    {
        return 1;
    }

    int idx = x + y * width;
    if (map[idx] == -1)
    {
        return 1;
    }

    int neighbors[8][2] = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},         {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };

    for (int i = 0; i < 8; i++)
    {
        int nx = x + neighbors[i][0];
        int ny = y + neighbors[i][1];

        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
        {
            if (map[nx + ny * width] == -1)
            {
                return 1;
            }
        }
    }

    return 0;
}


