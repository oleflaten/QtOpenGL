#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <QOpenGLFunctions_4_1_Core>

//#include "GL/glew.h" //We use QOpenGLFunctions instead

//This class is pretty much a copy of the shader class at
//https://github.com/SonarSystems/Modern-OpenGL-Tutorials/blob/master/%5BLIGHTING%5D/%5B8%5D%20Basic%20Lighting/Shader.h

class Shader : protected QOpenGLFunctions_4_1_Core
{
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader( const GLchar *vertexPath, const GLchar *fragmentPath );
    // Uses the current shader
    void Use( );
};

#endif
