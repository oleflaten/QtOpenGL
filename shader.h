#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <QOpenGLFunctions>

//#include "GL/glew.h" //We use QOpenGLFunctions instead

class Shader : protected QOpenGLFunctions
{
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader( const GLchar *vertexPath, const GLchar *fragmentPath );
    // Uses the current shader
    void Use( );
};

#endif
