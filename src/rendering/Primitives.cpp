#include "rendering/Primitives.hpp"

//taken from https://github.com/JoeyDeVries/Cell
Mesh Primitives::torus(float r1, float r2, int numSteps1, int numSteps2) {
	Mesh m;

    // NOTE(Joey): we generate an additional minor ring segment as we can't
    // directly connect to the first minor ring as the last set of vertices
    // require unique texture coordinates.
    // TODO(Joey): can probably merge these steps within the for-loop
    // conditions for readability (as we need to do numSteps + 1 at all
    // the relevant locations which is confusing and prone to bugs).
    m.positions.resize((numSteps1 + 1) * (numSteps2 + 1));
    m.normals.resize((numSteps1 + 1) * (numSteps2 + 1));
    m.uvs.resize((numSteps1 + 1) * (numSteps2 + 1));


    // NOTE(Joey): first we calculate all points for the major ring on the xy
    // plane (in default mathematics, the z-axis is considered the up axis).
    // TODO(Joey): no need really to pre-calculate these; integrate them
    // within the main for loops.
    std::vector<vec3> p(numSteps1 + 1);
    float a = 0.0f;
    float step = 2.0f * PI / numSteps1;
    for (int i = 0; i <= numSteps1; ++i)
    {
        float x = cos(a) * r1;
        float y = sin(a) * r1;
        p[i].x = x;
        p[i].y = y;
        p[i].z = 0.0f;
        a += step;
    }

    // NOTE(Joey): generate all the vertices, UVs, Normals (and Tangents/Bitangents):
    for (int i = 0; i <= numSteps1; ++i)
    {
        // NOTE(Joey): the basis vectors of the ring equal the difference
        // vector between the minorRing center and the donut's center
        // position (which equals the origin (0, 0, 0)) and the positive
        // z-axis.
        vec3 u = normalize(vec3() - p[i]) * r2; // Could be p[i] also
        vec3 v = vec3(0.0f, 0.0f, 1.0f) * r2;

        // NOTE(Joey): create the vertices of each minor ring segment:
        float a = 0.0f;
        float step = 2.0f * PI / numSteps2;
        for (int j = 0; j <= numSteps2; ++j)
        {
            float c = cos(a);
            float s = sin(a);

            m.positions[i * (numSteps2 + 1) + j] = p[i] + c * u + s * v;
            m.uvs[i * (numSteps2 + 1) + j].x = ((float)i) / ((float)numSteps1);
            m.uvs[i * (numSteps2 + 1) + j].y = ((float)j) / ((float)numSteps2);
            m.normals[i * (numSteps2 + 1) + j] = normalize(c * u + s * v);
            a += step;
        }
    }


    // NOTE(Joey): generate the indicies for a triangle topology:
    // NOTE(Joey): copied from gamedev.net resource.
    m.indices.resize(numSteps1 * numSteps2 * 6);

    int index = 0;
    for (int i = 0; i < numSteps1; ++i)
    {
        int i1 = i;
        int i2 = (i1 + 1);

        for (int j = 0; j < numSteps2; ++j)
        {
            int j1 = j;
            int j2 = (j1 + 1);

            m.indices[index++] = i1 * (numSteps2 + 1) + j1;
            m.indices[index++] = i1 * (numSteps2 + 1) + j2;
            m.indices[index++] = i2 * (numSteps2 + 1) + j1;

            m.indices[index++] = i2 * (numSteps2 + 1) + j2;
            m.indices[index++] = i2 * (numSteps2 + 1) + j1;
            m.indices[index++] = i1 * (numSteps2 + 1) + j2;
        }
    }
    m.generate();

    return m;
}

Mesh Primitives::sphere(unsigned int xSegments, unsigned int ySegments) {
	Mesh m;

	float PI = 3.141592;
	float TAU = PI * 2;

	for (unsigned int y = 0; y <= ySegments; ++y)
	{
		for (unsigned int x = 0; x <= xSegments; ++x)
		{
			float xSegment = (float)x / (float)ySegments;
			float ySegment = (float)y / (float)ySegments;
			float xPos = std::cos(xSegment * TAU) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * TAU) * std::sin(ySegment * PI);

			m.positions.push_back(vec3(xPos, yPos, zPos));
			m.uvs.push_back(vec2(xSegment, ySegment));
			m.normals.push_back(vec3(xPos, yPos, zPos));
			m.tangents.push_back(cross(vec3(xPos, yPos, zPos), vec3(0.0f, 1.0f, 0.0f)));
		}
	}

	for (unsigned int y = 0; y < ySegments; ++y)
	{
		for (unsigned int x = 0; x < xSegments; ++x)
		{
			m.indices.push_back((y + 1) * (xSegments + 1) + x);
			m.indices.push_back(y       * (xSegments + 1) + x);
			m.indices.push_back(y       * (xSegments + 1) + x + 1);

			m.indices.push_back((y + 1) * (xSegments + 1) + x);
			m.indices.push_back(y       * (xSegments + 1) + x + 1);
			m.indices.push_back((y + 1) * (xSegments + 1) + x + 1);
		}
	}

	m.generate();

	return m;
}

Mesh Primitives::sprite() {
	Mesh m;

	m.positions = std::vector<vec3>{
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f,  1.0f, 0.0f },

		{ 1.0f,  1.0f, 0.0f },
		{ 0.0f,  1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }

	};
	m.uvs = std::vector<vec2>{
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },

		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
		{ 0.0f, 0.0f },
	};

    m.generate();

    return m;
}

Mesh Primitives::framebuffer() {
	Mesh m;

	m.positions = std::vector<vec3>{
		{ -1.0f, -1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.0f },
		{ 1.0f,  1.0f, 0.0f },
		
		{ 1.0f,  1.0f, 0.0f },
		{ -1.0f,  1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.0f }
		
	};
	m.uvs = std::vector<vec2>{
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		
		
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
		{ 0.0f, 0.0f },
	};

	m.generate();

	return m;
}

Mesh Primitives::cube(float scale, float width, float height, float depth) {
	Mesh m;

    m.positions = std::vector<vec3> {
			{ 0.5f*width,  0.5f*height, -0.5f*depth},
			{ 0.5f*width, -0.5f*height, -0.5f*depth},
			{-0.5f*width, -0.5f*height, -0.5f*depth},
			{-0.5f*width, -0.5f*height, -0.5f*depth},
			{-0.5f*width,  0.5f*height, -0.5f*depth},
			{ 0.5f*width,  0.5f*height, -0.5f*depth},


			{-0.5f*width, -0.5f*height,  0.5f*depth},
			{ 0.5f*width, -0.5f*height,  0.5f*depth},
			{ 0.5f*width,  0.5f*height,  0.5f*depth},
			{ 0.5f*width,  0.5f*height,  0.5f*depth},
			{-0.5f*width,  0.5f*height,  0.5f*depth},
			{-0.5f*width, -0.5f*height,  0.5f*depth},

			{-0.5f*width,  0.5f*height,  0.5f*depth},
			{-0.5f*width,  0.5f*height, -0.5f*depth},
			{-0.5f*width, -0.5f*height, -0.5f*depth},
			{-0.5f*width, -0.5f*height, -0.5f*depth},
			{-0.5f*width, -0.5f*height,  0.5f*depth},
			{-0.5f*width,  0.5f*height,  0.5f*depth},


			{ 0.5f*width, -0.5f*height, -0.5f*depth},
			{ 0.5f*width,  0.5f*height, -0.5f*depth},
			{ 0.5f*width,  0.5f*height,  0.5f*depth},
			{ 0.5f*width,  0.5f*height,  0.5f*depth},
			{ 0.5f*width, -0.5f*height,  0.5f*depth},
			{ 0.5f*width, -0.5f*height, -0.5f*depth},


			{-0.5f*width, -0.5f*height, -0.5f*depth},
			{ 0.5f*width, -0.5f*height, -0.5f*depth},
			{ 0.5f*width, -0.5f*height,  0.5f*depth},
			{ 0.5f*width, -0.5f*height,  0.5f*depth},
			{-0.5f*width, -0.5f*height,  0.5f*depth},
			{-0.5f*width, -0.5f*height, -0.5f*depth},


			{ 0.5f*width,  0.5f*height,  0.5f*depth},
			{ 0.5f*width,  0.5f*height, -0.5f*depth},
			{-0.5f*width,  0.5f*height, -0.5f*depth},
			{-0.5f*width,  0.5f*height, -0.5f*depth},
			{-0.5f*width,  0.5f*height,  0.5f*depth},
			{ 0.5f*width,  0.5f*height,  0.5f*depth},
    };
    m.uvs = std::vector<vec2> {
            {0.0f*height*scale, 0.0f*width*scale},
            {1.0f*height*scale, 0.0f*width*scale},
            {1.0f*height*scale, 1.0f*width*scale},
            {1.0f*height*scale, 1.0f*width*scale},
            {0.0f*height*scale, 1.0f*width*scale},
            {0.0f*height*scale, 0.0f*width*scale},

            {0.0f*width*scale, 0.0f*height*scale},
            {1.0f*width*scale, 0.0f*height*scale},
            {1.0f*width*scale, 1.0f*height*scale},
            {1.0f*width*scale, 1.0f*height*scale},
            {0.0f*width*scale, 1.0f*height*scale},
            {0.0f*width*scale, 0.0f*height*scale},

            {1.0f*height*scale, 0.0f*depth*scale},
            {1.0f*height*scale, 1.0f*depth*scale},
            {0.0f*height*scale, 1.0f*depth*scale},
            {0.0f*height*scale, 1.0f*depth*scale},
            {0.0f*height*scale, 0.0f*depth*scale},
            {1.0f*height*scale, 0.0f*depth*scale},

            {1.0f*depth*scale, 0.0f*height*scale},
            {1.0f*depth*scale, 1.0f*height*scale},
            {0.0f*depth*scale, 1.0f*height*scale},
            {0.0f*depth*scale, 1.0f*height*scale},
            {0.0f*depth*scale, 0.0f*height*scale},
            {1.0f*depth*scale, 0.0f*height*scale},

            {0.0f*width*scale, 1.0f*depth*scale},
            {1.0f*width*scale, 1.0f*depth*scale},
            {1.0f*width*scale, 0.0f*depth*scale},
            {1.0f*width*scale, 0.0f*depth*scale},
            {0.0f*width*scale, 0.0f*depth*scale},
            {0.0f*width*scale, 1.0f*depth*scale},

            {0.0f*depth*scale, 1.0f*width*scale},
            {1.0f*depth*scale, 1.0f*width*scale},
            {1.0f*depth*scale, 0.0f*width*scale},
            {1.0f*depth*scale, 0.0f*width*scale},
            {0.0f*depth*scale, 0.0f*width*scale},
            {0.0f*depth*scale, 1.0f*width*scale},
    };
    m.normals = std::vector<vec3>{
            { 0.0f,  0.0f, -1.0f},
            { 0.0f,  0.0f, -1.0f},
            { 0.0f,  0.0f, -1.0f},
            { 0.0f,  0.0f, -1.0f},
            { 0.0f,  0.0f, -1.0f},
            { 0.0f,  0.0f, -1.0f},

            { 0.0f,  0.0f,  1.0f},
            { 0.0f,  0.0f,  1.0f},
            { 0.0f,  0.0f,  1.0f},
            { 0.0f,  0.0f,  1.0f},
            { 0.0f,  0.0f,  1.0f},
            { 0.0f,  0.0f,  1.0f},

            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},

            { 1.0f,  0.0f,  0.0f},
            { 1.0f,  0.0f,  0.0f},
            { 1.0f,  0.0f,  0.0f},
            { 1.0f,  0.0f,  0.0f},
            { 1.0f,  0.0f,  0.0f},
            { 1.0f,  0.0f,  0.0f},

            { 0.0f, -1.0f,  0.0f},
            { 0.0f, -1.0f,  0.0f},
            { 0.0f, -1.0f,  0.0f},
            { 0.0f, -1.0f,  0.0f},
            { 0.0f, -1.0f,  0.0f},
            { 0.0f, -1.0f,  0.0f},

            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f},
    };
	m.tangents = std::vector<vec3>{
			{ -1.0f,  0.0f, 0.0f},
			{ -1.0f,  0.0f, 0.0f},
			{ -1.0f,  0.0f, 0.0f},
			{ -1.0f,  0.0f, 0.0f},
			{ -1.0f,  0.0f, 0.0f},
			{ -1.0f,  0.0f, 0.0f},

			{ 1.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f},

			{ 0.0f,  0.0f,  -1.0f},
			{ 0.0f,  0.0f,  -1.0f},
			{ 0.0f,  0.0f,  -1.0f},
			{ 0.0f,  0.0f,  -1.0f},
			{ 0.0f,  0.0f,  -1.0f},
			{ 0.0f,  0.0f,  -1.0f},

			{ 0.0f,  0.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f},

			{ 1.0f, 0.0f,  0.0f},
			{ 1.0f, 0.0f,  0.0f},
			{ 1.0f, 0.0f,  0.0f},
			{ 1.0f, 0.0f,  0.0f},
			{ 1.0f, 0.0f,  0.0f},
			{ 1.0f, 0.0f,  0.0f},

			{ -1.0f,  0.0f,  0.0f},
			{ -1.0f,  0.0f,  0.0f},
			{ -1.0f,  0.0f,  0.0f},
			{ -1.0f,  0.0f,  0.0f},
			{ -1.0f,  0.0f,  0.0f},
			{ -1.0f,  0.0f,  0.0f},
	};

    m.generate();

    return m;
}

Mesh Primitives::skybox() {
	Mesh m;

    m.positions = std::vector<vec3> {
            {-0.5f, -0.5f, -0.5f},
            {0.5f,  -0.5f, -0.5f},
            {0.5f,  0.5f,  -0.5f},
            {0.5f,  0.5f,  -0.5f},
            {-0.5f, 0.5f,  -0.5f},
            {-0.5f, -0.5f, -0.5f},


            {0.5f,  0.5f,  0.5f},
            {0.5f,  -0.5f, 0.5f},
            {-0.5f, -0.5f, 0.5f},
            {-0.5f, -0.5f, 0.5f},
            {-0.5f, 0.5f,  0.5f},
            {0.5f,  0.5f,  0.5f},


            {-0.5f, -0.5f, -0.5f},
            {-0.5f, 0.5f,  -0.5f},
            {-0.5f, 0.5f,  0.5f},
            {-0.5f, 0.5f,  0.5f},
            {-0.5f, -0.5f, 0.5f},
            {-0.5f, -0.5f, -0.5f},


            {0.5f,  0.5f,  0.5f},
            {0.5f,  0.5f,  -0.5f},
            {0.5f,  -0.5f, -0.5f},
            {0.5f,  -0.5f, -0.5f},
            {0.5f,  -0.5f, 0.5f},
            {0.5f,  0.5f,  0.5f},


            {0.5f,  -0.5f, 0.5f},
            {0.5f,  -0.5f, -0.5f},
            {-0.5f, -0.5f, -0.5f},
            {-0.5f, -0.5f, -0.5f},
            {-0.5f, -0.5f, 0.5f},
            {0.5f,  -0.5f, 0.5f},


            {-0.5f, 0.5f,  -0.5f},
            {0.5f,  0.5f,  -0.5f},
            {0.5f,  0.5f,  0.5f},
            {0.5f,  0.5f,  0.5f},
            {-0.5f, 0.5f,  0.5f},
            {-0.5f, 0.5f,  -0.5f},


    };

    m.generate();

    return m;
}