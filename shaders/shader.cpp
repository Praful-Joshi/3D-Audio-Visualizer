#include "shader.h"
#include <GL/glew.h>
#include <iostream>

const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = 10.0;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

void main() {
    float dist = length(gl_PointCoord - vec2(0.5));
    if (dist > 0.5)
        discard;

    float alpha = 1.0 - smoothstep(0.4, 0.5, dist);
    FragColor = vec4(1.0, 0.5, 0.1, alpha); // orange with soft edges
}
)";

GLuint createShaderProgram(const char* vtxSrc, const char* fragSrc) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vtxSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSrc, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}