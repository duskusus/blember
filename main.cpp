#include "includes.hpp"
#include "testmodel.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stacktrace>
#include <glm/gtc/matrix_transform.hpp>
int main(int argc, char **argv) {
  Context c("blember", 1280, 720);
  Shader v("res/shaders/vertex.vert", GL_VERTEX_SHADER);
  Shader f("res/shaders/fragment.frag", GL_FRAGMENT_SHADER);
  Program p;
  p.attachShader(v);
  p.attachShader(f);
  p.link();
  p.use();
  float fov = glm::radians(90.0);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(1.0));
  model = glm::translate(model, glm::vec3(0.0, -50.0, -2.0));
  view = glm::translate(view, glm::vec3(0.0, 0, 0));
  glm::mat4 proj = glm::perspective(90.0, 1280.0/720.0, 0.01, 1000.0);
  Uniform u_model("model", p, (void *) glUniformMatrix4fv);
  Uniform u_view("view", p, (void *)glUniformMatrix4fv);
  Uniform u_proj("proj", p, (void *)glUniformMatrix4fv);
  u_view.set((void *)glm::value_ptr(view));
  u_proj.set((void *)glm::value_ptr(proj));
  GLfloat positions[] = {-1, -1, 0.0, 1, -1, 0.0, 1, 1, 0.0, -1, 1, 0.0};
  GLuint indices[] = {0, 1, 2, 2, 3, 0};
  entity e("res/models/world.obj", p);
  e.ready();
  while (c.poll() == 1) {
    e.render();
    c.swap();
  }
  return 0;
}