#include "rendering/Mesh.hpp"
#include <glad/glad.h>

#include <sstream>

Mesh::Mesh() {

}

Mesh::Mesh(const std::vector<vec3> &positions, const std::vector<unsigned int> &indices) {
    this->positions = positions;
    this->indices = indices;
}

Mesh::Mesh(const std::vector<vec3> &positions, const std::vector<vec3> &normals, const std::vector<unsigned int> &indices) {
    this->positions = positions;
    this->normals = normals;
    this->indices = indices;
}

Mesh::Mesh(const std::vector<vec3> &positions, const std::vector<vec3> &normals, const std::vector<vec2> &uvs, const std::vector<unsigned int> &indices) {
    this->positions = positions;
    this->normals = normals;
    this->uvs = uvs;
    this->indices = indices;
}

Mesh::Mesh(const std::vector<vec3> &positions, const std::vector<vec3> &normals, const std::vector<vec2> &uvs, const std::vector<vec3> &colors, const std::vector<unsigned int> &indices) {
    this->positions = positions;
    this->normals = normals;
    this->uvs = uvs;
    this->tangents = colors;
    this->indices = indices;
}

void Mesh::generate() {


    if (!VBO) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uvs.size() > 0)
        {
            data.push_back(uvs[i].x);
            data.push_back(uvs[i].y);
        }
        if (tangents.size() > 0)
        {
            data.push_back(tangents[i].x);
            data.push_back(tangents[i].y);
            data.push_back(tangents[i].z);
        }

        if (boneIDs.size() > 0) {
            // Since integers and floats both take up 4 bytes in opengl we can actually store an integer value in a float buffer.
            // This is crazy but it seems to work great.
            float f0;
            //*(unsigned int *)&f0 = boneIDs[i].x;
            memcpy(&f0, &boneIDs[i].x, 4);

            float f1;
            //*(unsigned int *)&f1 = boneIDs[i].y;
            memcpy(&f1, &boneIDs[i].y, 4);

            float f2;
            //*(unsigned int *)&f2 = boneIDs[i].z;
            memcpy(&f2, &boneIDs[i].z, 4);

            float f3;
            //*(unsigned int *)&f3 = boneIDs[i].w;
            memcpy(&f3, &boneIDs[i].w, 4);

            data.push_back(f0);
            data.push_back(f1);
            data.push_back(f2);
            data.push_back(f3);
        }

        if (boneWeights.size() > 0) {
            data.push_back(boneWeights[i].x);
            data.push_back(boneWeights[i].y);
            data.push_back(boneWeights[i].z);
            data.push_back(boneWeights[i].w);
        }
    }

    length = positions.size();

	if (data.size() > 0) {

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		if (indices.size() > 0)
		{
		    if (!EBO)
			    glGenBuffers(1, &EBO);


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			length = indices.size();
		}

		int stride = 3 * sizeof(float);
		if (uvs.size() > 0) stride += 2 * sizeof(float);
		if (normals.size() > 0) stride += 3 * sizeof(float);
		if (tangents.size() > 0) stride += 3 * sizeof(float);
		if (boneIDs.size() > 0) stride += 4 * sizeof(float);
		if (boneWeights.size() > 0) stride += 4 * sizeof(float);


		long offset = 0;
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
		offset += 3 * sizeof(float);

		if (normals.size() > 0)
		{
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
			offset += 3 * sizeof(float);
		}
		if (uvs.size() > 0)
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
			offset += 2 * sizeof(float);
		}
		if (tangents.size() > 0) {
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
			offset += 3 * sizeof(float);
		}
		if (boneIDs.size() > 0) {
		    glEnableVertexAttribArray(4);
		    glVertexAttribIPointer(4, 4, GL_INT, stride, (GLvoid*)offset);
            offset += 4 * sizeof(float);
		}
        if (boneWeights.size() > 0) {
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 4 * sizeof(float);
        }
	}
}

void Mesh::clear() {
    positions.clear();
    uvs.clear();
    normals.clear();
    tangents.clear();
    indices.clear();
}

void Mesh::render() const {
    glBindVertexArray(VAO);

    if (EBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glDrawElements(renderMode, length, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(renderMode, 0, length);
    }

    glBindVertexArray(0);
}

// https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
template<typename Out>
static void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            *(result++) = item;
    }
}

static std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}
// ------------------------------------------------------------------------------------------------

json Mesh::toJson() const {
    json j;

    std::string sPositions;
    for (size_t i = 0; i < positions.size(); i++) {
        sPositions += std::to_string(positions[i].x);
        sPositions += ",";
        sPositions += std::to_string(positions[i].y);
        sPositions += ",";
        sPositions += std::to_string(positions[i].z);
        sPositions += ",";
    }
    j["positions"] = sPositions;

    std::string sNormals;
    if (normals.size() > 0) {
        for (size_t i = 0; i < normals.size(); i++) {
            sNormals += std::to_string(normals[i].x);
            sNormals += ",";
            sNormals += std::to_string(normals[i].y);
            sNormals += ",";
            sNormals += std::to_string(normals[i].z);
            sNormals += ",";
        }
    }
    j["normals"] = sNormals;

    std::string sUvs;
    if (uvs.size() > 0) {
        for (size_t i = 0; i < uvs.size(); i++) {
            sUvs += std::to_string(uvs[i].x);
            sUvs += ",";
            sUvs += std::to_string(uvs[i].y);
            sUvs += ",";
        }
    }
    j["uvs"] = sUvs;

    std::string sTangents;
    if (tangents.size() > 0) {
        for (size_t i = 0; i < tangents.size(); i++) {
            sTangents += std::to_string(tangents[i].x);
            sTangents += ",";
            sTangents += std::to_string(tangents[i].y);
            sTangents += ",";
            sTangents += std::to_string(tangents[i].z);
            sTangents += ",";
        }
    }
    j["tangents"] = sTangents;


    std::string sIndices;
    if (indices.size() > 0) {
        for (size_t i = 0; i < indices.size(); i++) {
            sIndices += std::to_string(indices[i]);
            sIndices += ",";
        }
    }
    j["indices"] = sIndices;

    return j;
}

void Mesh::fromJson(const json &j) {
    clear();
    json jPositions = j["positions"];
    json jNormals = j["normals"];
    json jUvs = j["uvs"];
    json jTangents = j["tangents"];
    json jIndices = j["indices"];


    if (jPositions.is_string()) {
        std::vector<std::string> sPositions = split(jPositions, ',');

        for (size_t i = 0; i < sPositions.size(); i += 3) {
            positions.push_back(vec3(stof(sPositions[i]), stof(sPositions[i+1]), stof(sPositions[i+2])));
        }
    }
    if (jNormals.is_string()) {
        std::vector<std::string> sNormals = split(jNormals, ',');

        for (size_t i = 0; i < sNormals.size(); i += 3) {
            normals.push_back(vec3(stof(sNormals[i]), stof(sNormals[i+1]), stof(sNormals[i+2])));
        }
    }
    if (jUvs.is_string()) {
        std::vector<std::string> sUvs = split(jUvs, ',');

        for (size_t i = 0; i < sUvs.size(); i += 2) {
            uvs.push_back(vec2(stof(sUvs[i]), stof(sUvs[i+1])));
        }
    }
    if (jTangents.is_string()) {
        std::vector<std::string> sTangents = split(jTangents, ',');

        for (size_t i = 0; i < sTangents.size(); i += 3) {
            tangents.push_back(vec3(stof(sTangents[i]), stof(sTangents[i+1]), stof(sTangents[i+2])));
        }
    }
    if (jIndices.is_string()) {
        std::vector<std::string> sIndices = split(jIndices, ',');

        for (size_t i = 0; i < sIndices.size(); i++) {
            indices.push_back(stoi(sIndices[i]));
        }
    }
}

void Mesh::destroy() {
    clear();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
