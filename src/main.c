/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : main
 * @created     : Vendredi jan 03, 2025 20:10:13 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <time.h>

#include "gfx.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static float dt = 0;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static double get_time();

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
int main(void)
{
    gfx_init_opengl();

    GLFWwindow *window = gfx_get_window();

    // Variables for FPS calculation
    int frame_count = 0;
    float fps_timer = 0.0f;
    float fps;

    // Game loop
    double previous_time = get_time();
    while(!glfwWindowShouldClose(window))
    {
        double current_time = get_time();
        dt = current_time - previous_time;
        previous_time = current_time;

        glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // FPS Calculation
        frame_count++;
        fps_timer += dt;

        if (fps_timer >= 1.0) // Print every 1 second
        {
            fps = frame_count / fps_timer;
            printf("\rFPS: %.0f", fps);
            fflush(stdout);
            frame_count = 0;
            fps_timer = 0.0;
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

inline double get_time() 
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
