#include "includes.hpp"
#include "testmodel.hpp"
#include "testplane.hpp"
#include <SDL2/SDL_keycode.h>
#include <glm/glm.hpp>
#include <stacktrace>
#include <chrono>
#include "chunk.hpp"
int main(int argc, char **argv) {
  Context c("blember", 1920, 1080);
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



  const unsigned int worldSize = 4;
  const unsigned int chunkCount = worldSize * worldSize;
  Chunk **chunks = new Chunk*[chunkCount];
  for(int i = 0; i < chunkCount; i++) {
    chunks[i] = new Chunk(p, u_model);
    chunks[i]->move(i % worldSize, 0, i / worldSize);
  }
  auto start = std::chrono::steady_clock::now();
  auto last_frame = start;
  while (c.poll() == 1) {
    u_view.set((void *)glm::value_ptr(view));
    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - last_frame).count() / 1000000.0;
    last_frame = now;
    //std::cout << "Frame Time " << deltaTime << std::endl;
    float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count() / 1000000.0;
    u_time.set(&elapsed);
    u_model.set((void*)glm::value_ptr(glm::mat4(1.0f)));
    for(int i = 0; i < chunkCount; i++) {
      chunks[i]->render();
    }
    glm::vec4 move = glm::vec4(0.0);
    if (c.keys->at(SDLK_w)) {
      move.z += 1.0;
    }
    if (c.keys->at(SDLK_s)) {
      move.z += -1.0;
    }
    if (c.keys->at(SDLK_a)) {
      move.x += 1.0;
    }
    if (c.keys->at(SDLK_d)) {
      move.x -= 1.0;
    }
    if (c.keys->at(SDLK_SPACE)) {
      move.y -= 1.0;
    }
    if (c.keys->at(SDLK_LCTRL)) {
      move.y += 1.0;
    }
    if (c.keys->at(SDLK_ESCAPE)) {
      break;
    }
    view = glm::mat4(1.0);
    view = glm::rotate(view, c.mouse.y, glm::vec3(1.0, 0.0, 0.0));
    view = glm::rotate(view, c.mouse.x, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 inview = glm::inverse(view);
    move = inview * move;
    camera += move;
    view = glm::translate(view, glm::vec3(camera.x, camera.y, camera.z));
    c.swap();
  }
  return 0;
}