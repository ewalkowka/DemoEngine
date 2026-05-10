#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <iostream>

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // OpenGL 4.6
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
    );

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Temporary default window size before settings system is added
    int width = 1280;
    int height = 720;

    SDL_Window* window = SDL_CreateWindow(
        "Demo Engine",
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window)
    {
        std::cout << "SDL_CreateWindow failed: "
            << SDL_GetError() << std::endl;

        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    if (!glContext)
    {
        std::cout << "OpenGL context creation failed: "
            << SDL_GetError() << std::endl;

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    int gladVersion = gladLoadGL(SDL_GL_GetProcAddress);

    if (gladVersion == 0)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    std::cout << "Loaded OpenGL "
        << GLAD_VERSION_MAJOR(gladVersion)
        << "."
        << GLAD_VERSION_MINOR(gladVersion)
        << std::endl;

    // VSync
    SDL_GL_SetSwapInterval(1);

    bool running = true;

    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
        }

        SDL_GetWindowSizeInPixels(window, &width, &height);

        glViewport(0, 0, width, height);

        glClearColor(0.05f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}