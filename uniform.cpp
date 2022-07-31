
#include "uniform.hpp"

Uniform::Uniform(std::string name, Program &p_program, void *p_setFunc) : p(p_program) {
    location = glGetUniformLocation(p.getID(), name.c_str());
    setFunc = (void (*)(GLint, GLsizei, GLboolean, void*)) p_setFunc;
}
void Uniform::set(void *value) {
    setFunc(location, 1, GL_FALSE, value);
}