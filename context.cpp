#include "context.hpp"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "glError.hpp"
#include <GL/glu.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <iostream>
#include <string>
Context::Context(std::string windowName, uint16_t width, uint16_t height)
    : width(width), height(height) {
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
  glewInit();
  glEnable(GL_DEBUG_OUTPUT);
glDebugMessageCallback(MessageCallback, 0);
}
Uint64 frames = 0;
void Context::swap() {
  SDL_GL_SwapWindow(window);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  frames ++;
}
int Context::poll() {
  SDL_Event e;
  while(SDL_PollEvent(&e)) {
    if(e.type == SDL_QUIT) return 0;
  }
  return 1;
}
