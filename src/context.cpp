#include "context.hpp"
#include "glError.hpp"
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <iostream>
#include <string>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
Context::Context(std::string windowName, uint16_t width, uint16_t height)
    : width(width), height(height) {
  keycount = sizeof(keysToPoll) / sizeof(SDL_Keycode);
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, width, height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  context = SDL_GL_CreateContext(window);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  swap();
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetSwapInterval(1);
  glewInit();
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  //SDL_SetRelativeMouseMode(SDL_TRUE);

  keys = new std::unordered_map<SDL_Keycode, bool>;

  for (int i = 0; i < keycount; i++) {
    keys->insert({keysToPoll[i], false});
  }
}
Uint64 frames = 0;
void Context::swap() {
  SDL_GL_SwapWindow(window);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  frames++;
  if (showFrameInfo)
    std::cout << "Frame: " << frames << std::endl;
}
int Context::poll() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      return 0;
    else if (e.type == SDL_KEYDOWN) {
      for (unsigned int i = 0; i < keycount; i++) {
        if (e.key.keysym.sym == keysToPoll[i])
          keys->at(keysToPoll[i]) = true;
      }
    } else if (e.type == SDL_KEYUP) {
      for (unsigned int i = 0; i < keycount; i++) {
        if (e.key.keysym.sym == keysToPoll[i])
          keys->at(keysToPoll[i]) = false;
      }
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
mouse = glm::vec2(float(2.0 * mousex) / width * 2.0 - 1.0, float(2.0 * mousey) / height - 1.0) * sensitivity;
mouse.y = glm::clamp<float>(mouse.y, -glm::pi<float>()*0.5, glm::pi<float>()*0.5);
return 1;
}
