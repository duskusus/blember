#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <cstddef>
#include <string>

#include "context.h"

int nextPowerOf2(int x)
{
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
    int width = c.width;
    bool shouldUpdate = true;

    SDL_Color textColor = {0xff, 0xff, 0xff};
    SDL_Color backgroundColor = {0x00, 0x00, 0x00};

    uint32_t texId;
    GLuint fboId = 0;

   public:
    TextBox(const std::string &p_text, float p_x, float p_y, int p_size,
            Context &p_c)
        : size(p_size), text(p_text), x(p_x), y(p_y), c(p_c)
    {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glGenFramebuffers(1, &fboId);
    }
    void setfont(const std::string &p_fontpath) { 
        shouldUpdate = true;
        fontpath = p_fontpath; }
    void update()
    {
        shouldUpdate = false;
        if (font) TTF_CloseFont(font);
        font = TTF_OpenFont(fontpath.c_str(), size);

        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

        if (!font)
            printf("Error: Could not open font %s\n%s\n", fontpath.c_str(),
                   TTF_GetError());

        if (text_surface) SDL_FreeSurface(text_surface);
        text_surface = TTF_RenderText_Shaded_Wrapped(font, text.c_str(), textColor,
                                                  backgroundColor, width);
        text_surface =
            SDL_ConvertSurfaceFormat(text_surface, SDL_PIXELFORMAT_RGBA32, 0);
        
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, text_surface->w,
                     text_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     text_surface->pixels);

               glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, texId, 0);
    }
    void Draw()
    {
        if(shouldUpdate) update();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!text_surface) printf("Error: text surface is null!!\n");
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // if not already bound
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
        glBlitFramebuffer(0, 0, text_surface->w, text_surface->h, x, y,
                          x + text_surface->w, y - text_surface->h,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
    };
    void setText(const std::string &s) { 
        shouldUpdate = true;
        text = s; }
    void setPosition(int p_x, int p_y)
    {
        shouldUpdate = true;
        x = p_x;
        y = p_y;
    }
    void setFontSize(int p_size) { 
        shouldUpdate = true;
        size = p_size; }
    void setWidth(int p_width) { 
        shouldUpdate = true;
        width = p_width; }
    void setColor(SDL_Color fg, SDL_Color bg)
    {
        shouldUpdate = true;
        textColor = fg;
        backgroundColor = bg;
    }
    void append(std::string s)
    {
        text += s;
        update();
        if(text_surface->h > c.height) {
            text = " ";
        }
        shouldUpdate = true;
    }
    int getHeight() const {
        if(text_surface) return text_surface->h;
        return -1;
    }
    ~TextBox()
    {
        glDeleteTextures(1, &texId);
        glDeleteFramebuffers(1, &fboId);
    }
};