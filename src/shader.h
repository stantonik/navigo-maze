/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : shader
 * @created     : Samedi jan 04, 2025 23:30:09 CET
 */

#ifndef SHADER_H
#define SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "GL/glew.h"

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef enum
{
    SHADER_WORLD,
    SHADER_TEXT,
    SHADER_UI
} shader_type_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
extern void shader_init();
extern void shader_use(shader_type_t type);
extern GLuint shader_get_program(shader_type_t type);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* SHADER_H */

