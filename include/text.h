#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <cstddef>
#include <string>

#include "context.h"

int nextPowerOf2(int x) {
int v = x;
v--;
v |= v >> 1;
v |= v >> 2;
v |= v >> 4;
v |= v >> 8;
v |= v >> 16;
v++;
return v;
}

class TextBox {
    std::string text;
    std::string fontpath;
    float x;
    float y;
    int size;

    SDL_Surface *text_surface = nullptr;
    SDL_Texture *text_texture = nullptr;
    TTF_Font *font = nullptr;
    Context &c;

   public:
    TextBox(const std::string &p_text, float p_x, float p_y, int p_size,
            Context &p_c)
        : size(p_size), text(p_text), x(p_x), y(p_y), c(p_c)
    {
    }
    void setfont(const std::string &p_fontpath) { fontpath = p_fontpath; }
    void update()
    {
        if (font)TTF_CloseFont(font);
        font = TTF_OpenFont(fontpath.c_str(), size);
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
        if (!font)
            printf("Error: Could not open font %s\n%s\n", fontpath.c_str(),
                   TTF_GetError());
        SDL_Color textColor = {0xff, 0xff, 0xff};
        SDL_Color background = {0x00, 0x00, 0x00};
        if (text_surface) SDL_FreeSurface(text_surface);
            text_surface = TTF_RenderText_LCD_Wrapped(font, text.c_str(), textColor, background,  500);
            text_surface = SDL_ConvertSurfaceFormat(text_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    }
    void Draw()
    {
        glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uint32_t texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        if(!text_surface) printf("Error: text surface is null!!\n");
        const int trueWidth = text_surface->w;
        const int trueHeight = text_surface->h;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  text_surface->w, text_surface->h, 0,  GL_BGRA, GL_UNSIGNED_BYTE, text_surface->pixels);
        GLuint fboId = 0;
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, texId, 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // if not already bound
        glBlitFramebuffer(0, 0, trueWidth, trueHeight, x, y, x + trueWidth, y - trueHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glDisable(GL_BLEND);
        glDeleteTextures(1, &texId);
        glDeleteFramebuffers(1, &fboId);
    };
    void setText(const std::string &s) {
        text = s;
    }
    void setPosition(int p_x, int p_y) {
        x = p_x;
        y = p_y;
    }
    void setFontSize(int p_size) {
        size = p_size;
    }
};