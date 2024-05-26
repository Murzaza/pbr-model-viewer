#include "Framebuffer.hpp"
#include "World.hpp"
#include "debug.hpp"
#include <glad/glad.h>

Framebuffer::Framebuffer()
{
    init(scrnWidth, scrnHeight);
}

Framebuffer::Framebuffer(uint32_t width, uint32_t height)
{
    init(width, height);
}

void Framebuffer::init(uint32_t width, uint32_t height)
{
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    /* Setup texture */
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Attach texture to FrameBuffer */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);

    /* RBO */
    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        DEBUG("ERROR::FRAMEBUFFER Framebuffer is not complete\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    glDeleteRenderbuffers(1, &_rbo);
    glDeleteFramebuffers(1, &_fbo);
    glDeleteTextures(1, &_tex);
}

void Framebuffer::setFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void Framebuffer::unsetFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
