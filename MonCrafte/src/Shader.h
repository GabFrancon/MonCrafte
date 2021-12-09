#ifndef SHADER_H
#define SHADER_H

#include <algorithm>
#include <vector>
#include <memory>
#include <cmath>
#include <map>
#include <random>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader() : ID(0) {}
    // constructor generates the shader on the fly
    // -----------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath);
    // activate the shader
    // ------------------------------------------------------------------------
    void use();
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const;
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const;
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const;
    //-------------------------------------------------------------------------
    void setVec3(const std::string& name, glm::vec3 value) const;
    //-------------------------------------------------------------------------
    void setMat4(const std::string& name, glm::mat4 value) const;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif

