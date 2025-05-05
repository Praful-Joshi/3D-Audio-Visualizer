// Minimal 3D Particle Renderer - Step 1
// Renders a single 3D particle using OpenGL with GLFW + GLEW

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
//#include "audioAnalyzer.h"

const int particleCount = 500;
const float camRadius = 10.0f;
float camAngle = 0.0f;

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

// Function to compile shaders and create shader program
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

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "3D Particle", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Vertex data for particles
    std::vector<glm::vec3> particles;
    for (int i = 0; i < particleCount; ++i) {
        float x = ((rand() % 100) / 50.0f - 1.0f) * 2.0f;
        float y = ((rand() % 100) / 50.0f - 1.0f) * 2.0f;
        float z = ((rand() % 100) / 50.0f - 1.0f) * 7.0f;
        particles.push_back(glm::vec3(x, y, z));
    }
    std::vector<glm::vec3> originalParticles = particles;  // keep a copy

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec3), particles.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    GLuint shaderProgram = createShaderProgram(vertexShaderSrc, fragmentShaderSrc);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Projection and view matrix (static for now)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f/600.f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));

    //Audio Analyzer
    // AudioAnalyzer analyzer("assets/music.wav");
    // analyzer.init();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        //Animated camera position (orbiting)
        float time = glfwGetTime();

        // Update particle positions with sine wave animation
        for (size_t i = 0; i < particles.size(); ++i) {
            particles[i].y = originalParticles[i].y + sin(time + originalParticles[i].x * 3.0f) * 0.5f;
        }
        // Update VBO with new particle positions
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(glm::vec3), particles.data());

        float camX = sin(camAngle) * camRadius;
        float camZ = cos(camAngle) * camRadius;
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 0, camZ),
                                    glm::vec3(0, 0, 0),
                                    glm::vec3(0, 1, 0));

        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, particles.size());

        glfwSwapBuffers(window);
        glfwPollEvents();

        //Keyboard input
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            camAngle -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            camAngle += 0.01f;
        }
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glfwTerminate();
    return 0;
}
