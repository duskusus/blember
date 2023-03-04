#include "context.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <signal.h>

#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

#include "glError.h"
#include "newchunk.h"

Context::Context(std::string windowName, uint16_t width, uint16_t height)
    : width(width), height(height)
{
    keycount = sizeof(keysToPoll) / sizeof(SDL_Keycode);
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    context = SDL_GL_CreateContext(window);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) printf("Could not create renderer\n");
    glClearColor(0.4235, 0.851, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetSwapInterval(1);
    glewInit();
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_CULL_FACE);

    // SDL_SetRelativeMouseMode(SDL_TRUE);

    keys = new std::unordered_map<SDL_Keycode, bool>;

    for (int i = 0; i < keycount; i++) {
        keys->insert({keysToPoll[i], false});
    }
}
void Context::swap()
{
    SDL_GL_SwapWindow(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    frames++;
    if (showFrameInfo) std::cout << "Frame: " << frames << std::endl;
}
int Context::poll()
{
    LMouse = false;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            return 0;
        else if (e.type == SDL_KEYDOWN) {
            for (unsigned int i = 0; i < keycount; i++) {
                if (e.key.keysym.sym == keysToPoll[i])
                    keys->at(keysToPoll[i]) = true;
            }
            if (e.key.keysym.sym == SDLK_ESCAPE) return 0;
        }
        else if (e.type == SDL_KEYUP) {
            for (unsigned int i = 0; i < keycount; i++) {
                if (e.key.keysym.sym == keysToPoll[i])
                    keys->at(keysToPoll[i]) = false;
            }
        } 
        else if(e.type == SDL_MOUSEBUTTONDOWN) {
            LMouse = true;
        }
    }
    int mousex = 0;
    int mousey = 0;
    /*
    SDL_GetRelativeMouseState(&mousex, &mousey);
    SDL_WarpMouseInWindow(window, width / 2, height / 2);
    mouse += glm::vec2(float(mousex),float(mousey)) * sensitivity;
    */
    SDL_GetMouseState(&mousex, &mousey);
    mouse = glm::vec2(float(2.0 * mousex) / width * 2.0 - 1.0,
                      float(2.0 * mousey) / height - 1.0) *
            sensitivity;
    mouse.y = glm::clamp<float>(mouse.y, -glm::pi<float>() * 0.5,
                                glm::pi<float>() * 0.5);
    return 1;
}
void Context::fly_control_view(glm::mat4 &view, glm::vec4 &camera,
                               const float deltaTime)
{

    glm::vec4 move = glm::vec4(0.0);

    float positionIncrement = 0.25 * deltaTime / 0.01667;

    if (keys->at(SDLK_w)) {
        move.z += positionIncrement;
    }
    if (keys->at(SDLK_s)) {
        move.z -= positionIncrement;
    }
    if (keys->at(SDLK_a)) {
        move.x += positionIncrement;
    }
    if (keys->at(SDLK_d)) {
        move.x -= positionIncrement;
    }
    if (keys->at(SDLK_SPACE)) {
        move.y -= positionIncrement;
    }
    if (keys->at(SDLK_LCTRL)) {
        move.y += positionIncrement;
    }
    if (keys->at(SDLK_t)) {
        flying = false;
    }

    view = glm::mat4(1.0);
    view = glm::rotate(view, mouse.y, glm::vec3(1.0, 0.0, 0.0));
    view = glm::rotate(view, mouse.x, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 inview = glm::inverse(view);
    move = inview * move;
    camera -= move;
    view = glm::translate(view, - glm::vec3(camera.x, camera.y, camera.z));
}
void Context::walk_control_view(glm::mat4 &view, glm::vec4 &camera,
                                const float deltaTime, NewChunk &world)
{
    glm::vec4 move = glm::vec4(0.0);
    float positionIncrement = 5.0 * deltaTime;
    if (keys->at(SDLK_w)) {
        move.z += positionIncrement;
    }
    if (keys->at(SDLK_s)) {
        move.z -= positionIncrement;
    }
    if (keys->at(SDLK_a)) {
        move.x += positionIncrement;
    }
    if (keys->at(SDLK_d)) {
        move.x -= positionIncrement;
    }
    if (keys->at(SDLK_t)) {
        flying = true;
    }

    view = glm::mat4(1.0);
    view = glm::rotate(view, mouse.y, glm::vec3(1.0, 0.0, 0.0));
    view = glm::rotate(view, mouse.x, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 inview = glm::inverse(view);
    move = inview * move;

    camera += move;
    camera.y = -4.0 - world.getHeightmapVal(-int(camera.x + 0.5),
                                            -int(camera.z + 0.5));
    if (move.length() > 0.0)
        std::cout << "x " << camera.x << " y " << camera.y << " z " << camera.z
                  << std::endl;
    view = glm::translate(view, glm::vec3(camera.x, camera.y, camera.z));
}
