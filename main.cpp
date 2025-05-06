// 3D Audio Visualizer with OpenGL
// This program creates a 3D particle system that reacts to audio frequencies.

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <vector>
#include "audioAnalyzer/audioAnalyzer.h"
#include "shaders/shader.h"

const int particleCount = 500;
const float camRadius = 15.0f;
float camAngle = 0.0f;
std::vector<glm::vec3> originalParticles;
GLuint vao, vbo, shaderProgram;

void initPattern(std::vector<glm::vec3>& particles, std::vector<glm::vec3>& originalParticles) {
    for (int i = 0; i < particleCount; ++i) {
        float theta = glm::linearRand(0.0f, glm::two_pi<float>());
        float phi = glm::linearRand(0.0f, glm::pi<float>());
        float radius = glm::linearRand(0.5f, 3.0f);
    
        float x = radius * sin(phi) * cos(theta);
        float y = radius * sin(phi) * sin(theta);
        float z = radius * cos(phi);
    
        particles.push_back(glm::vec3(x, y, z));
    }
    originalParticles = particles;  // keep a copy
}

void setupGraphics(const std::vector<glm::vec3>& particles) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec3), particles.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    shaderProgram = createShaderProgram(vertexShaderSrc, fragmentShaderSrc);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "3D Particle", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_PROGRAM_POINT_SIZE);

    //Initialize particles object    
    std::vector<glm::vec3> particles;

    //Initialize pattern
    initPattern(particles, originalParticles);

    //Setup OpenGL
    setupGraphics(particles);

    // Projection and view matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f/600.f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));

    //Audio Analyzer
    AudioAnalyzer analyzer("assets/music.wav");
    analyzer.init();

    while (!glfwWindowShouldClose(window)) {
        analyzer.update();
        std::vector<float> bands = analyzer.getFrequencyBands();

        // Print first few bands (not all 64 every frame to avoid spam)
        std::cout << "Bands: ";
        for (int i = 0; i < 8; ++i) {
            std::cout << bands[i] << " ";
        }
        std::cout << std::endl;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // Update particle positions with breathe animation
        for (size_t i = 0; i < particles.size(); ++i) {
            float freqAmp = bands[i % bands.size()];
            particles[i].x = originalParticles[i].x * (1.0f + freqAmp * 1.5f);
            particles[i].y = originalParticles[i].y * (1.0f + freqAmp * 1.5f);
            particles[i].z = originalParticles[i].z * (1.0f + freqAmp * 1.5f);
        }
        // Update VBO with new particle positions
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(glm::vec3), particles.data());


        //Animated camera position (orbiting)
        float time = glfwGetTime();
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
