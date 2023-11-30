#pragma once
#include <cstdint>

class Framebuffer
{
public:
    Framebuffer();
    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer();

    unsigned int getFramebuffer() { return _fbo; };
    unsigned int getFramebufferTexture() { return _tex; };

    void setFramebuffer();
    void unsetFramebuffer();

private:
    void init(uint32_t width, uint32_t height);

    unsigned int _fbo;
    unsigned int _tex;
    unsigned int _rbo;
};