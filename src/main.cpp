#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

std::string LoadTextFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

unsigned int CompileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

        std::cout << "Shader compilation failed:\n"
            << infoLog << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

unsigned int CreateShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    std::string vertexShaderSource = LoadTextFile(vertexShaderPath);
    std::string fragmentShaderSource = LoadTextFile(fragmentShaderPath);

    if (vertexShaderSource.empty() || fragmentShaderSource.empty())
    {
        return 0;
    }

    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    if (vertexShader == 0 || fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog);

        std::cout << "Shader program linking failed:\n"
            << infoLog << std::endl;

        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

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

    unsigned int shaderProgram = CreateShaderProgram(
        "shaders/triangle.vert",
        "shaders/triangle.frag"
    );

    if (shaderProgram == 0)
    {
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    float vertices[] =
    {
        // x,y,z                 // r,g,b
        -0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f,      1.0f, 0.0f, 0.0f
    };

    unsigned int vao = 0;
    unsigned int vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        nullptr
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    auto startTime = std::chrono::high_resolution_clock::now();
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

        auto currentTime = std::chrono::high_resolution_clock::now();

        float time = std::chrono::duration<float>(
            currentTime - startTime
        ).count();

        glUseProgram(shaderProgram);

        int timeLocation = glGetUniformLocation(shaderProgram, "uTime");
        glUniform1f(timeLocation, time);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}