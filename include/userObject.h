#include <glm/glm.hpp>
#include "program.h"
#include "uniform.h"
#include ""
// abstract class for things that can render themselves, make opengl calls, go
// into space partitioning structures?
class UserObj {
    Uniform &u_mode;
    Program &p;
    UserObj();
    virtual void render() = 0;
    virtual void translate(glm::vec3 &movement) = 0;
};