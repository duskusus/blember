#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>

#include <chrono>
#include <glm/glm.hpp>
#include <string>

#include "chunk.h"
#include "context.h"
#include "includes.h"
#include "newchunk.h"
#include "newchunk2.h"
#include "testmodel.h"
#include "testplane.h"
#include "text.h"
#include "heightmap.h"

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
    Context c("blember", 1280, 720);
    TTF_Init();
    c.showFrameInfo = false;
    Shader v("res/shaders/fast.vert", GL_VERTEX_SHADER);
    Shader f("res/shaders/fast.frag", GL_FRAGMENT_SHADER);

    Program p;

    p.attachShader(v);
    p.attachShader(f);
    p.link();
    float fov = glm::radians(90.0);

    auto view = glm::mat4(1.0f);
    srand(time(NULL));
    view = glm::translate(view, glm::vec3(0.0, 0, 0));

    glm::mat4 proj = glm::perspective(90.0, 1280.0 / 720.0, 0.01, 100000.0);
    float waterlevel = 100.0;

    Uniform u_model("model", p, (void *)glUniformMatrix4fv);
    Uniform u_view("view", p, (void *)glUniformMatrix4fv);
    Uniform u_proj("proj", p, (void *)glUniformMatrix4fv);
    Uniform u_time("time", p, (void *)glUniform1f);

    u_proj.set((void *)glm::value_ptr(proj));
    glm::mat4 identity = glm::identity<glm::mat4>();
    u_model.set((void *)glm::value_ptr(identity));

    glm::vec4 camera = glm::vec4(0.0);

    TextBox debugInfo("Console", c.width - 400, c.height, 18, c);
    debugInfo.setfont("res/Helvetica.ttf");

    TextBox console("Console Text: ", 0, c.height, 18, c);
    console.setWidth(150);
    console.setfont("res/LiberationSans-Regular.ttf");
    console.setText("Console\n");
    console.setColor({255, 255, 255, 255}, {0, 0, 0, 0});

    const int seed = (int)time(NULL);
    console.append("Seed: " + std::to_string(seed) + "\n");
    console.Draw();
    c.swap();
    srand(seed);

    Heightmap h(512);
    h.generate();
    std::cout << "heightmap generated\n";
    NewChunk2 nc2(p);
    NewChunk2 nc3(p);
    nc2.loadFromHeightmap(h);
    nc3.loadFromHeightmap(h, 0, 256);
    std::cout << "heightmap loaded\n";
    nc2.prepare();
    nc3.prepare();
    std::cout << "prepared\n";
    p.use();
    auto start = std::chrono::steady_clock::now();
    auto last_frame = start;
    float elapsed;
    while (c.poll() == 1) {
        auto now = std::chrono::steady_clock::now();

        float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(
                              now - last_frame)
                              .count() /
                          1000000.0;
        last_frame = now;

        elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(now - start)
                .count() /
            1000000.0;
        if (c.flying) {
            c.fly_control_view(view, camera, deltaTime);
        }
        u_view.set((void *)glm::value_ptr(view));
        u_time.set(&elapsed);

        std::string positionInfo = "x: " + std::to_string(camera.x) +
                                   " y: " + std::to_string(camera.y) +
                                   " z: " + std::to_string(camera.z);
        debugInfo.setText("fps:  " + std::to_string(float(c.frames) / elapsed) +
                          "\n" + positionInfo);
        // render here
        u_model.set((void *) glm::value_ptr(identity));
        if(c.frames % 10 == 0 ) {
            nc2.loadFromHeightmap(h, c.frames / 10, 0);
            nc2.prepare();
        }
        nc2.render();
        u_model.set((void *)glm::value_ptr(glm::translate(identity, glm::vec3(0, 0, 255))));
        nc3.render();
        debugInfo.Draw();
        console.Draw();
        c.swap();
    }
    std::cout << "Average Frametime: " << elapsed * 1000 / c.frames
              << " framerate: " << c.frames / elapsed << std::endl;
    return 0;
}