#include "includes.h"b

#include <SDL2/SDL_keycode.h>
#include <glm/glm.hpp>
#include <stacktrace>
#include <chrono>
#include "fragPlane.hpp"
int main(int argc, char **argv) {
  Context c("blember", 1280, 720);
  c.showFrameInfo = false;
  Shader v("res/shaders/vertex.vert", GL_VERTEX_SHADER);
  Shader f("res/shaders/fragment.frag", GL_FRAGMENT_SHADER);
  Program p;
  p.attachShader(v);
  p.attachShader(f);
  p.link();
  p.use();
  float fov = glm::radians(90.0);
  auto view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0, 0, 0));
  glm::mat4 proj = glm::perspective(90.0, 1280.0 / 720.0, 0.01, 100000.0);
  Uniform u_model("model", p, (void *)glUniformMatrix4fv);
  Uniform u_view("view", p, (void *)glUniformMatrix4fv);
  Uniform u_proj("proj", p, (void *)glUniformMatrix4fv);
  Uniform u_time("time", p, (void *) glUniform1f);
  u_proj.set((void *)glm::value_ptr(proj));
  glm::vec4 camera = glm::vec4(5.0);

  FragPlane fp(p);

  auto start = std::chrono::steady_clock::now();
  auto last_frame = start;
  while (c.poll() == 1) {
    u_proj.set((void *) glm::value_ptr(glm::mat4(1.0)));
    u_view.set((void *)glm::value_ptr(glm::mat4(1.0)));
    u_model.set((void*)glm::value_ptr(glm::mat4(1.0f)));
    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - last_frame).count() / 1000000.0;
    last_frame = now;
    fp.render();
    //std::cout << "Frame Time " << deltaTime << std::endl;
    float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count() / 1000000.0;
    u_time.set(&elapsed);
    c.swap();
  }
  return 0;
}