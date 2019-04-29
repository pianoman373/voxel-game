#pragma once

#include "Math.hpp"
#include <string>
#include "Path.hpp"

class Texture {
private:
    unsigned int id = 0;
public:

    void load(const unsigned char *data, int width, int height, bool pixelated=false, bool singleChannel=false);

    void loadFromSingleColor(const vec4 &color);

    void bind(unsigned int unit = 0) const;

    unsigned int getID() const;

	void setID(unsigned int id);

	void destroy();

	static void bindNull(unsigned int unit = 0);
};

class Cubemap {
private:
    unsigned int id = 0;

public:
    void load(const Path &file1, const Path &file2, const Path &file3, const Path &file4,
			  const Path &file5, const Path &file6);

	void loadEquirectangular(const Path &file, int resolution = 512);

    void bind(unsigned int unit = 0) const;

    unsigned int getID() const;

	void setID(unsigned int id);
};
