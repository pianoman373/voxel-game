class Framebuffer {
private:
    int fbo;
    int rbo;
    int texture;
    int width;
    int height;

public:
    void setupShadow(int width, int height);

    void bind();

    void bindTexture();

    int getTextureID();
};
