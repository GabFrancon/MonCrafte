#ifndef TEXT2D_H
#define TEXT2D_H

#include "Geometry.h"

class Text2D
{
public:
    Text2D() {};
    void initText2D(const char* text, unsigned int length, int x, int y, int size, GLuint texture)
    {
        font = texture;

        for (unsigned int i = 0; i < length; i++) {

            glm::vec2 vertex_up_left = glm::vec2(x + i * size, y + size);
            glm::vec2 vertex_up_right = glm::vec2(x + i * size + size, y + size);
            glm::vec2 vertex_down_right = glm::vec2(x + i * size + size, y);
            glm::vec2 vertex_down_left = glm::vec2(x + i * size, y);

            vertices.push_back(vertex_up_left[0]);
            vertices.push_back(vertex_up_left[1]);

            vertices.push_back(vertex_down_left[0]);
            vertices.push_back(vertex_down_left[1]);

            vertices.push_back(vertex_up_right[0]);
            vertices.push_back(vertex_up_right[1]);

            vertices.push_back(vertex_down_right[0]);
            vertices.push_back(vertex_down_right[1]);

            vertices.push_back(vertex_up_right[0]);
            vertices.push_back(vertex_up_right[1]);

            vertices.push_back(vertex_down_left[0]);
            vertices.push_back(vertex_down_left[1]);

            char character = text[i];
            float uv_x = ((int)character % 16) / 16.0f;
            float uv_y = ((float)character / 16) / 16.0f;

            glm::vec2 uv_up_left = glm::vec2(uv_x, 1.0f - uv_y);
            glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f/16, 1.0f - uv_y);
            glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f/16, 1.0f - (uv_y + 1.0f/16));
            glm::vec2 uv_down_left = glm::vec2(uv_x, 1.0f - (uv_y + 1.0f/16));

            UVs.push_back(uv_up_left[0]);
            UVs.push_back(uv_up_left[1]);

            UVs.push_back(uv_down_left[0]);
            UVs.push_back(uv_down_left[1]);

            UVs.push_back(uv_up_right[0]);
            UVs.push_back(uv_up_right[1]);

            UVs.push_back(uv_down_right[0]);
            UVs.push_back(uv_down_right[1]);

            UVs.push_back(uv_up_right[0]);
            UVs.push_back(uv_up_right[1]);

            UVs.push_back(uv_down_left[0]);
            UVs.push_back(uv_down_left[1]);
        }

        // generate the VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // position coordinates
        size_t vertexBufferSize = sizeof(float) * vertices.size();
        glGenBuffers(1, &posVbo);
        glBindBuffer(GL_ARRAY_BUFFER, posVbo);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coordinates
        size_t textureBufferSize = sizeof(float) * UVs.size();
        glGenBuffers(1, &texVbo);
        glBindBuffer(GL_ARRAY_BUFFER, texVbo);
        glBufferData(GL_ARRAY_BUFFER, textureBufferSize, UVs.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(2);

        // deactivate the VAO for now
        glBindVertexArray(0);
    }
	void printText2D()
	{
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}

    void cleanupText2D()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &posVbo);
        glDeleteBuffers(1, &texVbo);
    }
private:
    GLuint vao = 0;
    GLuint posVbo = 0;
    GLuint texVbo = 0;
    GLuint font = 0;
	std::vector<float> vertices;
	std::vector<float> UVs;
};

#endif //!TEXT2D_H