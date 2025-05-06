#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

extern const char* vertexShaderSrc;
extern const char* fragmentShaderSrc;

GLuint createShaderProgram(const char* vtxSrc, const char* fragSrc);

#endif // SHADER_H