#pragma once
#include <string>
#include "program.h"
class Uniform {
    unsigned int location;
    Program &p;
    void (*setFunc)(GLint location, GLsizei count, GLboolean transpose, void *value);
    public:
    Uniform(std::string name, Program &p_program, void *p_setFunc);
    void set(void *value);
};