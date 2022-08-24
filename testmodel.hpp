#include "ibo.hpp"
#include "program.hpp"
#include "vertexbuffer.hpp"
#include <assert.h>
#include <bits/types/FILE.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>
#include <glm/ext/vector_float3.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include "uniform.hpp"
class entity {
public:
  std::string data = "";
  std::vector<unsigned int> indices;
  std::vector<float> vertices;
  Program &p;
  VertexBuffer *v;
  IBO *i;
  Uniform &u_model;

  glm::mat4 model = glm::mat4(1.0f);

  entity(std::string objpath, Program &m_p, Uniform &u_model) : p(m_p), u_model(u_model) {
    std::fstream f;
    f.open(objpath);
    std::vector<std::string> lines;
    if (f.is_open()) {
      std::string line;
      while (getline(f, line)) {
        lines.push_back(line);
      }
    } else {
      std::cout << "unable to open " << objpath << std::endl;
    }
    for (int i = 0; i < lines.size(); i++) {
      const std::string &line = lines[i];
      std::vector<std::string> words;
      boost::split(words, line, boost::is_any_of(" "));
      std::vector<std::string> parts;
      std::vector<std::string> results;
      if (words[0] == "f") {
        for (auto word : words) {
          boost::split(parts, word, boost::is_any_of("/"));
          results.insert(results.end(), parts.begin(), parts.end());
        }
        int indicesOffset = -1;
        indices.push_back(std::stoi(results[1]) + indicesOffset);
        indices.push_back(std::stoi(results[4]) + indicesOffset);
        indices.push_back(std::stoi(results[7]) + indicesOffset);
      } else if (words[0] == "v") {
        vertices.push_back(stof(words[1]));
        vertices.push_back(stof(words[2]));
        vertices.push_back(stof(words[3]));
      }
    }
    std::cout << "{";
    for(auto &i : indices) {
      std::cout << i << ", ";
    }
    std::cout << "}" << std::endl;
    std::cout << "{";
    for(auto &i : vertices) {
      std::cout << i << ", ";
    }
    std::cout << "}" << std::endl;
  }
  void ready() {
    i = new IBO(&indices[0], indices.size());
    v = new VertexBuffer("aPos", p);
    v->set(&vertices[0], vertices.size());
    v->vertexSpec(3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    v->enable();
    v->upload();
    v->bind();
  }
  void render() {
    p.use();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    u_model.set(glm::value_ptr(model));
    v->bind();
    i->bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
  void move(glm::vec3 movement) {
    model = glm::translate(model, movement);
  }
  void scale(glm::vec3 scale) {
    model = glm::scale(model, scale);
  }
};
