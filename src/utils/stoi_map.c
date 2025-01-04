/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : stoi_map
 * @created     : Vendredi déc 27, 2024 17:06:41 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "stoi_map.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
void stoi_map_init(stoi_map_t *map) 
{
    map->entries = NULL;
}

void stoi_map_insert(stoi_map_t *map, const char *key, int value) 
{
    stoi_map_entry_t *entry = (stoi_map_entry_t *)malloc(sizeof(stoi_map_entry_t));
    entry->key = strdup(key);
    entry->value = value;
    HASH_ADD_KEYPTR(hh, map->entries, entry->key, strlen(entry->key), entry);
}

int stoi_map_get(stoi_map_t *map, const char *key, int *value) 
{
    stoi_map_entry_t *entry;
    HASH_FIND_STR(map->entries, key, entry);
    if (entry) {
        if (value != NULL)
        {
            *value = entry->value;
        }
        return 1;
    }

    return 0;
}

void stoi_map_remove(stoi_map_t *map, const char *key)
{
    stoi_map_entry_t *entry;
    HASH_FIND_STR(map->entries, key, entry);
    if (entry) {
        HASH_DEL(map->entries, entry);
        free(entry->key);
        free(entry);
    }
}

void stoi_map_destroy(stoi_map_t *map)
{
    stoi_map_entry_t *entry, *tmp;
    HASH_ITER(hh, map->entries, entry, tmp) {
        HASH_DEL(map->entries, entry);
        free(entry->key);
        free(entry);
    }
}

void stoi_map_print(stoi_map_t *map)
{
    stoi_map_entry_t *entry;
    for (entry = map->entries; entry != NULL; entry = entry->hh.next)
    {
        printf("{%s: %d}\n", entry->key, entry->value);
    }
}

