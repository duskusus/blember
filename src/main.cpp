#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>

#include <chrono>
#include <glm/glm.hpp>
#include <string>

#include "chunk.h"
#include "context.h"
#include "heightmap.h"
#include "includes.h"
#include "newchunk.h"
#include "newchunk2.h"
#include "testmodel.h"
#include "testplane.h"
#include "text.h"

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
    
    const int chunk_count = 2;

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

    Uniform u_pvm("pvm", p, (void *)glUniformMatrix4fv);
    Uniform u_time("time", p, (void *)glUniform1f);

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

    Heightmap h(1024);
    h.readFromFile("testmap2");

    camera.x = 0;
    camera.z = 0;
    camera.y = *h.getHeightmapPtr(int(camera.x), int(camera.y));

    std::vector<NewChunk2 *> chunks;

    for (int i = 0; i < chunk_count; i++) {

        chunks.push_back(new NewChunk2(p));
        chunks[i]->loadFromHeightmap(h, 256 * (i % 4), 256 * (i / 4));
        chunks[i]->prepare();

    }

    p.use();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

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
            1e6;

        if (c.flying) {
            //c.fly_control_view(view, camera, deltaTime);
            c.fly_control_view(view, camera, deltaTime);
        }

        u_time.set(&elapsed);

        std::string positionInfo = "x: " + std::to_string(camera.x) +
                                   " y: " + std::to_string(camera.y) +
                                   " z: " + std::to_string(camera.z);
        debugInfo.setText("fps:  " + std::to_string(float(c.frames) / elapsed) +
                          "\n" + positionInfo);
        // render here
        glm::mat4 identity = glm::identity<glm::mat4>();
        glm::mat4 model;
        glm::mat4 pvm;

        for (int i = 0; i < chunk_count; i++) {
            model = glm::translate(
                identity, glm::vec3(256.0 * (i % 4), 0, 256.0 * (i / 4)));
            pvm = proj * view * model;
            u_pvm.set((void *) glm::value_ptr(pvm));
            chunks[i]->render();
        }

        debugInfo.Draw();
        console.Draw();

        c.swap();
    }

    std::cout << "Average Frametime: " << elapsed * 1000 / c.frames
              << " framerate: " << c.frames / elapsed << std::endl;
    return 0;
}