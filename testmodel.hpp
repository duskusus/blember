#include <bits/types/FILE.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include <glm/vec3.hpp>
class entity {

    std::string data = "";
    public:
    entity(std::string objpath) {
        FILE *f = fopen(objpath.c_str(), "rb");
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *s = (char*) malloc(size+1);
        fread(s, size, 1, f);
        fclose(f);
        s[size] = 0;
        data = s;
        std::vector<std::string> lines;
        boost::split(lines, data, boost::is_any_of("\n"));
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;
        for(int i = 0; i < lines.size(); i++) {
            std::vector<std::string> line;
            std::vector<std::string> word;
            boost::split(line, lines[i], boost::is_any_of(" "));
            boost::split(word, line[1], boost::is_any_of("/"));
            if (line[0] == "v") {
                //line is vertex
                vertices.push_back(glm::vec3(std::stof(word[0]), std::stof(word[1]), std::stof(word[2])));
            } else if (line[0] == "f") {
                //line is index
                indices.push_back(std::stoi(word[0]));
                indices.push_back(std::stoi(word[1]));
                indices.push_back(std::stoi(word[2]));
            }
        }
    }
    void render() {
        //draw yourself!
    };
};