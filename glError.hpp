#pragma once
#include <SDL2/SDL_opengl.h>
void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *userParam);