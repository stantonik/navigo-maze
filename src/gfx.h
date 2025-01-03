/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : gfx
 * @created     : Vendredi jan 03, 2025 19:41:44 CET
 */

#ifndef GFX_H
#define GFX_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "GL/glew.h"
#include "GLFW/glfw3.h"

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
extern void gfx_init_opengl();
extern GLFWwindow *gfx_get_window();
extern GLuint gfx_get_shader_program();

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* GFX_H */

