/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : stoi_map
 * @created     : Vendredi déc 27, 2024 17:05:25 CET
 */

#ifndef STOI_MAP_H
#define STOI_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "uthash.h"

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct
{
    char *key;
    int value;
    UT_hash_handle hh;
} stoi_map_entry_t;

typedef struct
{
    stoi_map_entry_t *entries;
} stoi_map_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
void stoi_map_init(stoi_map_t *map);
void stoi_map_insert(stoi_map_t *map, const char *key, int value);
int stoi_map_get(stoi_map_t *map, const char *key, int *value);
void stoi_map_remove(stoi_map_t *map, const char *key);
void stoi_map_destroy(stoi_map_t *map);
void stoi_map_print(stoi_map_t *map);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* STOI_MAP_H */

