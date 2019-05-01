#pragma once

#include <vector>
#include "rendering/Texture.hpp"

class Framebuffer {
private:
   
    unsigned int rbo = 0;
    int width;
    int height;

	std::vector<Texture> attachments;
	int numAttachments = 0;

public:
	unsigned int fbo = 0;

	void setup(int width, int height);

	void attachTexture(unsigned int internalFormat, unsigned int format, unsigned int type);

	/**
	 * Attaches a depth and stencil buffer.
	 */
	void attachRBO();

    /**
     * Initializes this framebuffer with data for shadow maps.
     */
    void attachShadow(int width, int height);

    /**
     * Binds the framebuffer (makes all render calls draw do it.
     */
    void bind() const;

	const Texture &getAttachment(int num) const;

    int getWidth() const;

    int getHeight() const;

    void destroy();
};
