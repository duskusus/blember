#include "ibo.hpp"
#include "program.hpp"
#include "vertexbuffer.hpp"
#include <assert.h>
#include <bits/types/FILE.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
class entity {
public:
  std::string data = "";
  std::vector<unsigned int> indices;
  std::vector<float> vertices;
  Program &p;
  VertexBuffer *v;
  IBO *i;

  entity(std::string objpath, Program &m_p) : p(m_p) {
    FILE *f = fopen(objpath.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *s = (char *)malloc(size + 1);
    fread(s, size, 1, f);
    fclose(f);
    s[size] = 0;
    data = s;
    std::vector<std::string> lines;
    boost::split(lines, data, boost::is_any_of("\n"));
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
        indices.push_back(stoi(results[1]));
        indices.push_back(stoi(results[4]));
        indices.push_back(stoi(results[7]));
        indices.push_back(stoi(results[10]));
      } else if (words[0] == "v") {
        vertices.push_back(stof(words[1]));
        vertices.push_back(stof(words[2]));
        vertices.push_back(stof(words[3]));
      }
    }
  }
  void ready() {
    i = new IBO(&indices[0], indices.size());
    v = new VertexBuffer("aPos", p);
    v->set(&vertices[0], vertices.size());
    v->vertexSpec(3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    v->enable();
    v->upload();
    v->bind();
    // for(auto i : indices) {std::cout << i << "\n";}
    // for(auto v : vertices) {std::cout << v << "\n";}
  }
  void render() {
    p.use();
    glEnable(GL_DEPTH_TEST);
    v->bind();
    glDrawElements(GL_TRIANGLES, indices.size() / 4, GL_UNSIGNED_INT, &indices[0]);
  }
};
