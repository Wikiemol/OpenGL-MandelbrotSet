#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>
#include <fstream>
#include <string.h>
#include <math.h>

const int SCREEN_WIDTH = 768;
const int SCREEN_HEIGHT = 768;

SDL_Window *gWindow;

void fatalError(const char* message) {
    fprintf(stderr, "%s\n", message);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    exit(1);
}

int main() {

    { //-Initializing SDL Window and Context-//
        gWindow = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

        SDL_GLContext context = SDL_GL_CreateContext(gWindow);
    }

    { //-Initializing GLEW-//
        glewExperimental = true;
        GLuint err = glewInit();
        if(err != GLEW_OK) {
            const char* message = (char *) glewGetErrorString(err);
            fprintf(stderr, "Failed To Initialize Glew.\nGlew Error: ");
            fatalError(message);
        }
    }


    //-Setting up program-//
    GLuint programID = glCreateProgram();

    //-Setting up Buffers-//
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

    GLuint vertexBufferObjectID = 0;
    glGenBuffers(1, &vertexBufferObjectID);

    //-Creating Shaders-//
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    if(vertexShaderID == 0) {
        const char* message = "Failed to create fragment shader.";
        fatalError(message);
    }

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    if(fragmentShaderID == 0) {
        fatalError("Failed to create fragment shader.");
    }


    {//- compiling vertexShader -//
        std::ifstream vertexShaderFile("vertexShader.vert");
        if(vertexShaderFile.fail()) {
            const char* message = "Failed to load vertex shader.";
            fatalError(message);
        }
        std::string fullFile = "";
        std::string line;
        while(std::getline(vertexShaderFile, line)) {
            fullFile += line + "\n";
        }
        vertexShaderFile.close();

        const char* fileContents = fullFile.c_str();
        glShaderSource(vertexShaderID, 1, &fileContents, NULL);
        glCompileShader(vertexShaderID);

        GLint compileStatus;
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
        if(compileStatus == GL_FALSE) {
            GLint infoLogLength;
            glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            char errorMessage[infoLogLength];
            glGetShaderInfoLog(vertexShaderID, infoLogLength, &infoLogLength, errorMessage);
            fprintf(stderr, "Vertex Shader failed to compile.\n");
            fatalError(errorMessage);
        }
    }

    {//- compiling fragmentShader -//
        std::ifstream fragmentShaderFile("fragmentShader.frag");
        std::string fullFile = "";
        std::string line; while(std::getline(fragmentShaderFile, line)) { fullFile += line + "\n"; }
        fragmentShaderFile.close();
        const char* fileContents = fullFile.c_str();

        glShaderSource(fragmentShaderID, 1, &fileContents, NULL);
        glCompileShader(fragmentShaderID);

        GLint compileStatus;
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
        if(compileStatus == GL_FALSE) {
            GLint infoLogLength;
            glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            char errorMessage[infoLogLength];
            glGetShaderInfoLog(fragmentShaderID, infoLogLength, &infoLogLength, errorMessage);
            fprintf(stderr, "Fragment Shader failed to compile.\n");
            fatalError(errorMessage);
        }
    }

    float positions[12];
    positions[0] = -1;
    positions[1] = -1;

    positions[2] = -1;
    positions[3] = 1;

    positions[4] = 1;
    positions[5] = 1;

    positions[6] = -1;
    positions[7] = -1;

    positions[8] = 1;
    positions[9] = -1;

    positions[10] = 1;
    positions[11] = 1;

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID);
    glBufferData(GL_ARRAY_BUFFER, sizeof (positions), positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int numAttributes = 0;
    glBindAttribLocation(programID, numAttributes++, "position");
    glBindAttribLocation(programID, numAttributes++, "color");

    { //-Linking Program-//
        glAttachShader(programID, vertexShaderID);
        glAttachShader(programID, fragmentShaderID);
        glLinkProgram(programID);
        GLint linkStatus;
        glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);

        if(linkStatus == GL_FALSE) {
            GLint infoLogLength;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
            char infoLog[infoLogLength];
            glGetProgramInfoLog(programID, infoLogLength, &infoLogLength, infoLog);

            glDeleteProgram(programID);
            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);
            fprintf(stderr, "Failed to link program.\n");
            fatalError(infoLog);
        }

        glDetachShader(programID, vertexShaderID);
        glDetachShader(programID, fragmentShaderID);
    }

    SDL_Event e;
    bool quit = false;

    float time = 0;
    while(!quit) {
        while(SDL_PollEvent(&e) != 0) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.scancode == 41) {
                        quit = true;
                    }
                    break;
            }
        }


        glUseProgram(programID);

        GLuint zPowerLocation = glGetUniformLocation(programID, "zPower");
        if(zPowerLocation == GL_INVALID_INDEX) {
            fatalError("Uniform not found.");
        }

        glUniform1f(zPowerLocation, cos(time) + 3);
        glEnableVertexAttribArray(0);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectID);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(0);
        glDisableVertexAttribArray(0);

        SDL_GL_SwapWindow(gWindow);
        time += 0.01;
    }

    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}
