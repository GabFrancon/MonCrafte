#ifndef TEXT2D_H
#define TEXT2D_H

#include "Shader.h"

class Text2D
{
public:
    Text2D() {}

    Text2D(const char* text, unsigned int length, glm::vec2 position, int size, GLuint texture)
    {
        glGenVertexArrays(1, &vao);
        font = texture;
        this->length = length;
        offset = position;
        this->size = size;

        for (unsigned int i = 0; i < length; i++)
        {
            glm::vec2 vertex_down_left = glm::vec2(i * size, 0);
            glm::vec2 vertex_down_right = glm::vec2((i + 1) * size, 0);
            glm::vec2 vertex_up_left = glm::vec2(i * size, size);
            glm::vec2 vertex_up_right = glm::vec2((i + 1) * size, size);

            vertexPositions.push_back(vertex_up_left[0]);
            vertexPositions.push_back(vertex_up_left[1]);

            vertexPositions.push_back(vertex_down_left[0]);
            vertexPositions.push_back(vertex_down_left[1]);

            vertexPositions.push_back(vertex_up_right[0]);
            vertexPositions.push_back(vertex_up_right[1]);

            vertexPositions.push_back(vertex_down_right[0]);
            vertexPositions.push_back(vertex_down_right[1]);

            vertexPositions.push_back(vertex_up_right[0]);
            vertexPositions.push_back(vertex_up_right[1]);

            vertexPositions.push_back(vertex_down_left[0]);
            vertexPositions.push_back(vertex_down_left[1]);

            int lineNb = (int)text[i] / 16;
            int columnNb = (int)text[i] % 16;

            float uv_x = columnNb / 16.0;
            float uv_y = lineNb / 16.0;
            float step = 1 / 16.0;

            glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y + step);
            glm::vec2 uv_down_right = glm::vec2(uv_x + step, uv_y + step);
            glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
            glm::vec2 uv_up_right = glm::vec2(uv_x + step, uv_y);

            vertexTextures.push_back(uv_up_left[0]);
            vertexTextures.push_back(uv_up_left[1]);

            vertexTextures.push_back(uv_down_left[0]);
            vertexTextures.push_back(uv_down_left[1]);

            vertexTextures.push_back(uv_up_right[0]);
            vertexTextures.push_back(uv_up_right[1]);

            vertexTextures.push_back(uv_down_right[0]);
            vertexTextures.push_back(uv_down_right[1]);

            vertexTextures.push_back(uv_up_right[0]);
            vertexTextures.push_back(uv_up_right[1]);

            vertexTextures.push_back(uv_down_left[0]);
            vertexTextures.push_back(uv_down_left[1]);
        }
    };

    void updateText(const char* text)
    {
        vertexTextures.clear();

        for (unsigned int i = 0; i < length; i++)
        {
            int lineNb = (int)text[i] / 16;
            int columnNb = (int)text[i] % 16;

            float uv_x = columnNb / 16.0;
            float uv_y = lineNb / 16.0;
            float step = 1 / 16.0;

            glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y + step);
            glm::vec2 uv_down_right = glm::vec2(uv_x + step, uv_y + step);
            glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
            glm::vec2 uv_up_right = glm::vec2(uv_x + step, uv_y);

            vertexTextures.push_back(uv_up_left[0]);
            vertexTextures.push_back(uv_up_left[1]);

            vertexTextures.push_back(uv_down_left[0]);
            vertexTextures.push_back(uv_down_left[1]);

            vertexTextures.push_back(uv_up_right[0]);
            vertexTextures.push_back(uv_up_right[1]);

            vertexTextures.push_back(uv_down_right[0]);
            vertexTextures.push_back(uv_down_right[1]);

            vertexTextures.push_back(uv_up_right[0]);
            vertexTextures.push_back(uv_up_right[1]);

            vertexTextures.push_back(uv_down_left[0]);
            vertexTextures.push_back(uv_down_left[1]);
        }
    }

    void bindVBOs()
    {
        clearVBOs();

        glBindVertexArray(vao);

        // position coordinates
        size_t vertexBufferSize = sizeof(float) * vertexPositions.size();
        glGenBuffers(1, &posVbo);
        glBindBuffer(GL_ARRAY_BUFFER, posVbo);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexPositions.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coordinates
        size_t textureBufferSize = sizeof(float) * vertexTextures.size();
        glGenBuffers(1, &texVbo);
        glBindBuffer(GL_ARRAY_BUFFER, texVbo);
        glBufferData(GL_ARRAY_BUFFER, textureBufferSize, vertexTextures.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void render()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertexPositions.size());
    }

    void deleteVAO()
    {
        glDeleteVertexArrays(1, &vao);
    }

    void clearVBOs()
    {
        glDeleteBuffers(1, &posVbo);
        glDeleteBuffers(1, &texVbo);
    }

    glm::vec2 getOffset() {return offset;}
    unsigned int getLength() { return length; }
    int getSize() { return size; }

private:
    glm::vec2 offset;
    unsigned int length = 0;
    int size = 0;

    std::vector<float> vertexPositions;
    std::vector<float> vertexTextures;

    GLuint vao = 0;
    GLuint posVbo = 0;
    GLuint texVbo = 0;
    GLuint font = 0;

};

#endif //!TEXT2D_H