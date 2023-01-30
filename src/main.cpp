#include <SDL2/SDL_keycode.h>

#include <chrono>
#include <glm/glm.hpp>

#include "chunk.h"
#include "includes.h"
#include "newchunk.h"
#include "testmodel.h"
#include "testplane.h"

float frand(float min = 0.0, float max = 1.0)
{
    float f = float(rand()) / float(RAND_MAX);
    return f * (max - min) + min;
}

glm::vec3 v3rand()
{
    return glm::vec3(frand(0, 100), frand(0, 100), frand(0, 100));
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    Context c("blember", 1920, 1080);
    c.showFrameInfo = false;
    Shader v("res/shaders/clean.vert", GL_VERTEX_SHADER);
    Shader f("res/shaders/clean.frag", GL_FRAGMENT_SHADER);

    Program p;

    p.attachShader(v);
    p.attachShader(f);
    p.link();

    float fov = glm::radians(90.0);

    auto view = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0, 0, 0));

    glm::mat4 proj = glm::perspective(90.0, 1280.0 / 720.0, 0.01, 100000.0);

    Uniform u_model("model", p, (void *)glUniformMatrix4fv);
    Uniform u_view("view", p, (void *)glUniformMatrix4fv);
    Uniform u_proj("proj", p, (void *)glUniformMatrix4fv);
    Uniform u_time("time", p, (void *)glUniform1f);

    u_proj.set((void *)glm::value_ptr(proj));
    glm::mat4 identity = glm::identity<glm::mat4>();
    u_model.set((void *)glm::value_ptr(identity));

    glm::vec4 camera = glm::vec4(0.0);

    NewChunk nc(p, u_model,  800);
    nc.generate();
    int max = 0;
    for(int i = 0; i < nc.renderableBlockCount; i++) {
        if(nc.at(i).type > max) max = nc.at(i).type;
    }
    std::cout << "Min: " << max << std::endl;
    nc.sync();

    auto start = std::chrono::steady_clock::now();
    auto last_frame = start;
    while (c.poll() == 1) {
        c.swap();

        auto now = std::chrono::steady_clock::now();

        float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(
                              now - last_frame)
                              .count() /
                          1000000.0;
        last_frame = now;

        float elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(now - start)
                .count() /
            1000000.0;
        u_time.set(&elapsed);

        // render here 
        nc.render();

        if(c.keys->at(SDLK_r)) {
            srand(time(NULL));
            nc.generate();
            nc.sync();
        }

        if(c.flying){
            c.fly_control_view(view, camera, deltaTime);
            }else{
                c.walk_control_view(view, camera, deltaTime, nc);
            }
        u_view.set((void *)glm::value_ptr(view));

    }
    return 0;
}