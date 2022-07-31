#include "includes.hpp"

int main(int argc, char **argv) {
  Context c("gaming simulator 2069", 1280, 720);
  Shader v("res/shaders/vertex.vert", GL_VERTEX_SHADER);
  Shader f("res/shaders/fragment.frag", GL_FRAGMENT_SHADER);
  Program p;
  p.attachShader(v);
  p.attachShader(f);
  p.link();
  p.use();
  float fov = glm::radians(90.0);
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0, 0.0, -0.11));
  glm::mat4 proj = glm::mat4(1.0f);
  proj = glm::perspective(fov, 1280.0f / 720.0f, 0.1f, 100.0f);

  Uniform u_view("view", p, (void *)glUniformMatrix4fv);
  Uniform u_proj("proj", p, (void *)glUniformMatrix4fv);
  u_view.set((void *)glm::value_ptr(view));
  u_proj.set((void *)glm::value_ptr(proj));
  GLfloat positions[] = {-1, -1, 0.0, 1, -1, 0.0, 1, 1, 0.0, -1, 1, 0.0};
  GLuint indices[] = {0, 1, 2, 2, 3, 0};
  IBO OIndices(indices, 6);
  VertexAttrib vpos("aPos", p);
  vpos.buffer();
  vpos.vertexSpec(3, GL_FLOAT, GL_FALSE, 0);
  vpos.set(positions, 12 * sizeof(float));
  vpos.enable();
  vpos.upload();
  OIndices.bind();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
  c.swap();
  while (c.poll() == 1) {
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    c.swap();
  }
  return 0;
}