/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : maps
 * @created     : Samedi jan 04, 2025 15:01:53 CET
 */

#ifndef MAPS_H
#define MAPS_H

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "utils/vector.h"

#include <stdint.h>

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct
{
    uint16_t id;
    int x, y;
    bool is_border;
} tile_t;

typedef struct
{
    uint16_t width, height;
    vector_t tiles;
} map_t;

typedef enum
{
    MAP_ZEBRACROSSING,
    MAP_LABYRINTH,
    MAP_TRAIN,
    MAP_END,
} map_id_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
extern map_t map_get(map_id_t id);
extern void map_free(map_t map);

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MAPS_H */

