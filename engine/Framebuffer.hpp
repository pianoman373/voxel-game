#pragma once

class Framebuffer {
private:
    int fbo;
    int rbo;
    int texture;
    int width;
    int height;

public:
    /**
     * Initializes this framebuffer with data for shadow maps.
     */
    void setupShadow(int width, int height);

    /**
     * Binds the actual framebuffer (makes all render calls draw do it.
     */
    void bind();

    /**
     * Binds the texture of this framebuffer just like binding a normal Texture object.
     */
    void bindTexture(int unit = 0);

    int getTextureID();

    int getWidth();

    int getHeight();
};
