#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#include <stb_image.h>      // Image loading Utility functions
#include <fstream>
#include <sstream>

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//stb libary
#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"


// Vertex Shader Source Code
const GLchar* vertexShaderSource = R"(
#version 410 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 aPos;      // Position attribute
layout(location = 1) in vec3 aColor;    // Color attribute
layout(location = 2) in vec2 aTexCoord; // Texture coordinate attribute

out vec3 vertexColor; // Output variable to fragment shader
out vec2 TexCoord;    // Output variable for texture coordinates

uniform mat4 bookModel;   // Model matrix from application
uniform mat4 penModel;   // Model matrix from application
uniform mat4 cupModel;   // Model matrix from application
uniform mat4 glassesModel;   // Model matrix from application
uniform mat4 view;    // View matrix from application
uniform mat4 projection; // Projection matrix from application

void main() {
    // Combine matrices to transform the vertex position
    gl_Position = projection * view * bookModel * penModel * cupModel * glassesModel *vec4(aPos, 1.0);

    vertexColor = aColor; // Pass color to fragment shader
    TexCoord = aTexCoord; // Pass texture coordinates to fragment shader
}
)";

const GLchar* fragmentShaderSource = R"(
#version 410 core
#extension GL_ARB_separate_shader_objects : enable

in vec3 vertexColor; // Input variable from vertex shader
in vec2 TexCoord;    // Input variable for texture coordinates

out vec4 FragColor;  // Output variable: final color of the fragment

uniform sampler2D textureSampler1; // Texture sampler
uniform sampler2D textureSampler2; // Texture sampler
uniform sampler2D textureSampler3; // Texture sampler
uniform sampler2D textureSampler4; // Texture sampler

void main() {
    // Sample the texture using the texture coordinates
    vec4 textureColor = texture(textureSampler1, TexCoord);

    // Combine the texture color with the interpolated vertex color
    vec4 finalColor = textureColor * vec4(vertexColor, 1.0);

    // Output the final color of the fragment
    FragColor = finalColor;
}
)";

//shader program
GLuint gProgramId = 0;
//texture id
GLuint bookTextureId = 0;
GLuint penTextureId = 0;
GLuint glassesTextureId = 0;
GLuint cupTextureId = 0;

GLuint bookVAO, bookVBO, bookEBO;
GLuint penVAO, penVBO, penEBO;
GLuint glassesVAO, glassesVBO, glassesEBO;
GLuint cupVAO, cupVBO, cupEBO;

float windowWidth = 800;
float windowHeight = 600;

float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

float nearPlane = 0.1f;     // closest plan to the view
float farPlane = 100.0f;    // farthis plan in the view
float fov = 45.0f;          // Field of View in degrees




GLfloat bookVertices[] = {
    // Front face
    -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
    0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
    0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-right
    -0.5f, 0.5f, 0.5f,     1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-left

    // Back face
    0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
    -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-left
    0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-right

    // Top face
    0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Top-right
    -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Top-left
    -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Bottom-left
    0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Bottom-right

    // Bottom face
    -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Top-left
    0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Top-right
    0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Bottom-left

    // Left face
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
    -0.5f, -0.5f, 0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
    -0.5f, 0.5f, 0.5f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-left
    -0.5f, 0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-right

    // Right face
    0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
    0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
    0.5f, 0.5f, -0.5f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-right
    0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.0f,    0.0f, 1.0f  // Top-left
};

GLushort bookIndices[] = {
    // Indices for a cube representing a book
    0, 1, 2,  2, 3, 0,     // Front face
    4, 5, 6,  6, 7, 4,     // Back face
    8, 9, 10, 10, 11, 8,  // Top face
    12, 13, 14, 14, 15, 12,  // Bottom face
    16, 17, 18, 18, 19, 16,  // Left face
    20, 21, 22, 22, 23, 20   // Right face
};

const GLsizei bookVerticesSize = sizeof(bookVertices);
const GLsizei bookIndicesSize = sizeof(bookIndices);


// Glasses
GLfloat glassesVertices[] = {
    // Front face
       -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
       0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-right
       -0.5f, 0.5f, 0.5f,     1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-left

       // Back face
       0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
       -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
       -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // Top-left
       0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // Top-right

       // Top face
       0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Top-right
       -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Top-left
       -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // Bottom-left
       0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // Bottom-right

       // Bottom face
       -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Top-left
       0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Top-right
       0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Bottom-right
       -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Bottom-left

       // Left face
       -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
       -0.5f, -0.5f, 0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
       -0.5f, 0.5f, 0.5f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-left
       -0.5f, 0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-right

       // Right face
       0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
       0.5f, 0.5f, -0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f, // Top-right
       0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.0f,     1.0f, 1.0f  // Top-left
};

GLushort glassesIndices[] = {
    // Indices for glasses (a rectangular cube)...
    0, 1, 2, 2, 3, 0,     // Front face
    4, 5, 6, 6, 7, 4,     // Back face
    8, 9, 10, 10, 11, 8,  // Top face
    12, 13, 14, 14, 15, 12,  // Bottom face
    16, 17, 18, 18, 19, 16,  // Left face
    20, 21, 22, 22, 23, 20   // Right face
};

const GLsizei glassesVerticesSize = sizeof(glassesVertices);
const GLsizei glassesIndicesSize = sizeof(glassesIndices);

// Teacup
GLfloat cupVertices[] = {
    // Front face
       -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
       0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-right
       -0.5f, 0.5f, 0.5f,     1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-left

       // Back face
       0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
       -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
       -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // Top-left
       0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // Top-right

       // Top face
       0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Top-right
       -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Top-left
       -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // Bottom-left
       0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // Bottom-right

       // Bottom face
       -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Top-left
       0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Top-right
       0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Bottom-right
       -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Bottom-left

       // Left face
       -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
       -0.5f, -0.5f, 0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
       -0.5f, 0.5f, 0.5f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-left
       -0.5f, 0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-right

       // Right face
       0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
       0.5f, 0.5f, -0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f, // Top-right
       0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.0f,     1.0f, 1.0f  // Top-left
};

GLushort cupIndices[] = {
    // Indices for teacup (a rectangular cube)...
    0, 1, 2, 2, 3, 0,     // Front face
    4, 5, 6, 6, 7, 4,     // Back face
    8, 9, 10, 10, 11, 8,  // Top face
    12, 13, 14, 14, 15, 12,  // Bottom face
    16, 17, 18, 18, 19, 16,  // Left face
    20, 21, 22, 22, 23, 20   // Right face
};

const GLsizei cupVerticesSize = sizeof(cupVertices);
const GLsizei cupIndicesSize = sizeof(cupIndices);

// Pen
GLfloat penVertices[] = {
    // Front face
       -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
       0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-right
       -0.5f, 0.5f, 0.5f,     1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-left

       // Back face
       0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
       -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
       -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // Top-left
       0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // Top-right

       // Top face
       0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Top-right
       -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Top-left
       -0.5f, 0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // Bottom-left
       0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // Bottom-right

       // Bottom face
       -0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Top-left
       0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Top-right
       0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Bottom-right
       -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Bottom-left

       // Left face
       -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
       -0.5f, -0.5f, 0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
       -0.5f, 0.5f, 0.5f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Top-left
       -0.5f, 0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top-right

       // Right face
       0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Bottom-right
       0.5f, 0.5f, -0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f, // Top-right
       0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.0f,     1.0f, 1.0f  // Top-left
};

GLushort penIndices[] = {
    // Indices for pen (a rectangular cube)...
    0, 1, 2, 2, 3, 0,     // Front face
    4, 5, 6, 6, 7, 4,     // Back face
    8, 9, 10, 10, 11, 8,  // Top face
    12, 13, 14, 14, 15, 12,  // Bottom face
    16, 17, 18, 18, 19, 16,  // Left face
    20, 21, 22, 22, 23, 20   // Right face
};

const GLsizei penVerticesSize = sizeof(penVertices);
const GLsizei penIndicesSize = sizeof(penIndices);

// Function to log error messages to a file
void LogError(const std::string& errorMessage) {
    std::ofstream errorLog("error_log.txt", std::ios::app); // Opens log file for appending
    if (errorLog.is_open()) {
        errorLog << errorMessage << std::endl;
        errorLog.close();
    }
    else {
        // If unable to open log file, output to console
        std::cerr << "Error: Unable to open log file." << std::endl;
        
    }

    std::cerr << errorMessage << std::endl;
}

void LogError(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[512];

    if (type == "SHADER_COMPILATION") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::string errorMessage = "Shader compilation error: " + std::string(infoLog);
            LogError(errorMessage);
        }
    }
    else if (type == "PROGRAM_LINKING") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            std::string errorMessage = "Program linking error: " + std::string(infoLog);
            LogError(errorMessage);
        }
    }
    else {
        std::string errorMessage = "Unknown error type.";
        LogError(errorMessage);
    }
}





GLuint CreateShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource) {
    // Vertex Shader Compilation
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    LogError(vertexShader, "SHADER_COMPILATION");

    // Fragment Shader Compilation
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    LogError(fragmentShader, "SHADER_COMPILATION");

    // Shader Program Linking
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    LogError(shaderProgram, "PROGRAM_LINKING");

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}




// Function to compile shader
GLuint compileShader(GLenum type, const GLchar* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for shader compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR: Shader compilation failed:\n" << infoLog << std::endl;
        return 0;
    }

    return shader;
}

// Function to create shader program
GLuint createShaderProgram(const GLchar* vertexShaderSrc, const GLchar* fragmentShaderSrc) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR: Vertex shader compilation failed:\n" << infoLog << std::endl;
        return 0;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR: Fragment shader compilation failed:\n" << infoLog << std::endl;
        return 0;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR: Shader program linking failed:\n" << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


bool createTexture(const char* filename, GLuint& textureId) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image) {
        // Flip image vertically as OpenGL has the origin at the bottom-left corner
        stbi_set_flip_vertically_on_load(true);// im making you look at the opposte side of the cube so this is off for now

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        }
        else if (channels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        else {
            std::cout << "Not implemented to handle image with " << channels << " channels" << std::endl;
            stbi_image_free(image);
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    std::cout << "Failed to load texture " << filename << std::endl;
    return false;
}

void destroyTexture(GLuint textureId) {
    glDeleteTextures(1, &textureId);
}

void generateObjectBuffers(GLuint& vao, GLuint& vbo, GLuint& ebo, const GLfloat* vertices, const GLushort* indices, GLsizei verticesSize, GLsizei indicesSize) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

    // Define the vertex attribute pointers
    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Function to bind texture to a specific sampler
void bindTexture(GLuint textureId, GLuint textureUnit, const char* uniformName, GLuint programId, const char* samplerName) {
    glActiveTexture(GL_TEXTURE0 + textureUnit); // Activate texture unit before binding
    glBindTexture(GL_TEXTURE_2D, textureId);    // Bind the texture to the active unit

    // Set the sampler to the corresponding texture unit
    glUniform1i(glGetUniformLocation(programId, samplerName), textureUnit);

    // Bind the texture to the shader uniform
    glUniform1i(glGetUniformLocation(programId, uniformName), textureUnit);
}

glm::mat4 createProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 createViewMatrix(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 up) {
    return glm::lookAt(cameraPosition, cameraTarget, up);
}

glm::mat4 createModelMatrix(glm::vec3 translation, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scale) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, translation);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}

// Example: GLFW window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}


GLFWwindow* makeWindow() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
       
    }

    // GLFW: set version, profile, and window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
      
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

   
    return window;
}

int loadGLEW() {
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    return 0;
}

GLuint loadProgram(int& programState) {
    GLuint gProgramId = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (gProgramId == 0) {
        std::cerr << "Failed to create shader program." << std::endl;
        programState = -1;
    }

    return gProgramId;
}

glm::mat4 loadViewMatrix() {
    glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    return  createViewMatrix(cameraPosition, cameraTarget, up);
}

glm::mat4 initModleMatrixs() {
    glm::vec3 position(0.0f, 0.0f, 0.0f);
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);
    float rotationAngle = 45.0f;
    glm::vec3 scale(1.0f, 1.0f, 1.0f);
    return  createModelMatrix(position, rotationAxis, rotationAngle, scale);
}

void loadTextures() {
    if (!createTexture("..\\book.png", bookTextureId)) {
        std::cerr << "failed to load book texture" << std::endl;
    }
    if (!createTexture("..\\cup.png", cupTextureId)) {
        std::cerr << "failed to load cup texture" << std::endl;
    }
    if (!createTexture("..\\glasses.png", glassesTextureId)) {
        std::cerr << "failed to load glasses texture" << std::endl;
    }
    if (!createTexture("..\\pen.png", penTextureId)) {
        std::cerr << "failed to load pen texture" << std::endl;
    }
}

void setupObject(GLuint& VAO, GLuint& VBO, GLfloat vertices[], int vertexCount) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount, vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    // GLFW window creation
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Drawing Objects", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, windowWidth, windowHeight);

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Link shaders into a shader program
    gProgramId = glCreateProgram();
    glAttachShader(gProgramId, vertexShader);
    glAttachShader(gProgramId, fragmentShader);
    glLinkProgram(gProgramId);

    // Check if shader compilation and linking were successful
    GLint success;
    glGetProgramiv(gProgramId, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(gProgramId, 512, nullptr, infoLog);
        std::string errorMessage = "Shader linking error: ";
        errorMessage += infoLog;
        LogError(errorMessage);
    }

    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //start the shader program
    glUseProgram(gProgramId);

    // Define the model, view, and projection matrices
    glm::mat4 bookModel = glm::mat4(1.0f); // Identity matrix for the model
    glm::mat4 penModel = glm::mat4(1.0f); // Identity matrix for the model
    glm::mat4 glassesModel = glm::mat4(1.0f); // Identity matrix for the model
    glm::mat4 cupModel = glm::mat4(1.0f); // Identity matrix for the model
    glm::mat4 view = glm::mat4(1.0f); // Identity matrix for the view
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Get the uniform locations from the shader program
    GLint bookModelLoc = glGetUniformLocation(gProgramId, "bookModel");
    GLint penModelLoc = glGetUniformLocation(gProgramId, "penModel");
    GLint glassesModelLoc = glGetUniformLocation(gProgramId, "glassesModel");
    GLint cupModelLoc = glGetUniformLocation(gProgramId, "cupModel");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projectionLoc = glGetUniformLocation(gProgramId, "projection");

    // Pass matrices to the shader
    glUniformMatrix4fv(bookModelLoc, 1, GL_FALSE, glm::value_ptr(bookModel));
    glUniformMatrix4fv(penModelLoc, 1, GL_FALSE, glm::value_ptr(penModel));
    glUniformMatrix4fv(glassesModelLoc, 1, GL_FALSE, glm::value_ptr(glassesModel));
    glUniformMatrix4fv(cupModelLoc, 1, GL_FALSE, glm::value_ptr(cupModel));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Set up buffers for each object
    GLuint bookVAO, bookVBO;
    setupObject(bookVAO, bookVBO, bookVertices, sizeof(bookVertices));

    GLuint penVAO, penVBO;
    setupObject(penVAO, penVBO, penVertices, sizeof(penVertices));

    GLuint glassesVAO, glassesVBO;
    setupObject(glassesVAO, glassesVBO, glassesVertices, sizeof(glassesVertices));

    GLuint cupVAO, cupVBO;
    setupObject(cupVAO, cupVBO, cupVertices, sizeof(cupVertices));

    // gens VAO
    glGenVertexArrays(1, &bookVAO);
    glGenVertexArrays(1, &penVAO);
    glGenVertexArrays(1, &glassesVAO);
    glGenVertexArrays(1, &cupVAO);

    // gets vbo id
    glGenBuffers(1, &bookVBO);
    glGenBuffers(1, &penVBO);
    glGenBuffers(1, &glassesVBO);
    glGenBuffers(1, &cupVBO);

    // gets ebo id
    glGenBuffers(1, &bookEBO);
    glGenBuffers(1, &penEBO);
    glGenBuffers(1, &glassesEBO);
    glGenBuffers(1, &cupEBO);

    glBindVertexArray(bookVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bookVBO);
    glBufferData(GL_ARRAY_BUFFER, bookVerticesSize, bookVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bookEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bookIndicesSize, bookIndices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture Coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind bookVAO

    // Bind Vertex Array Object for pen
    glBindVertexArray(penVAO);

    glBindBuffer(GL_ARRAY_BUFFER, penVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(penVertices), penVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, penEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(penIndices), penIndices, GL_STATIC_DRAW);

    // Set attribute pointers for pen object
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Bind Vertex Array Object for glasses
    glBindVertexArray(glassesVAO);

    glBindBuffer(GL_ARRAY_BUFFER, glassesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glassesVertices), glassesVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glassesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glassesIndices), glassesIndices, GL_STATIC_DRAW);

    // Set attribute pointers for glasses object
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Bind Vertex Array Object for cup
    glBindVertexArray(cupVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cupVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cupVertices), cupVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cupEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cupIndices), cupIndices, GL_STATIC_DRAW);

    // Set attribute pointers for cup object
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Load textures 
    if (!createTexture("..\\book.png", bookTextureId)) {
        std::cerr << "failed to load book texture" << std::endl;
    }
    if (!createTexture("..\\cup.png", cupTextureId)) {
        std::cerr << "failed to load cup texture" << std::endl;
    }
    if (!createTexture("..\\glasses.png", glassesTextureId)) {
        std::cerr << "failed to load glasses texture" << std::endl;
    }
    if (!createTexture("..\\pen.png", penTextureId)) {
        std::cerr << "failed to load pen texture" << std::endl;
    }

    // set texture uniforms
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler1"), 0); //book texture is bound to texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler2"), 1); // the pen texture is bound to texture unit 1
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler3"), 2); //  the glasses texture is bound to texture unit 2
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler4"), 3); // the cup texture is bound to texture unit 3

    // Translate the book along the X-axis by 2 units
    bookModel = glm::translate(bookModel, glm::vec3(2.0f, 0.0f, 0.0f));

    // Translate the pen along the X-axis by -2 units (opposite direction of the book)
    penModel = glm::translate(penModel, glm::vec3(-1.0f, 0.0f, 0.0f));

    // Translate the glasses along the Z-axis by 2 units
    glassesModel = glm::translate(glassesModel, glm::vec3(0.0f, 0.0f, 2.0f));

    // Translate the cup along the Z-axis by -2 units (opposite direction of the glasses)
    cupModel = glm::translate(cupModel, glm::vec3(0.0f, 0.0f, -2.0f));


    // sets the camera speed
    float cameraSpeed = .005f;
    float rotationSpeed = 1.0f;

    // Define initial camera position, target, and camera up vector
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Define initial Euler angles for rotation
    float yaw = -90.0f; // Initial yaw angle (facing towards -Z axis)
    float pitch = 0.0f; // Initial pitch angle (horizontal plane)

 
   

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Poll for events
        glfwPollEvents();

        // Clear buffers
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(gProgramId);

        //messing with the view materix to move camera
        // Calculate the front vector using Euler angles
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        glm::vec3 cameraFront = glm::normalize(front);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition += cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            pitch += rotationSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            pitch -= rotationSpeed;
        }

        // Update the view matrix based on the new camera position and target
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Render book
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bookTextureId);

        glBindVertexArray(bookVAO);
        glDrawElements(GL_TRIANGLES, bookVerticesSize, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        // Render pen
        glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
        glBindTexture(GL_TEXTURE_2D, penTextureId); // Bind pen texture
   
        glBindVertexArray(penVAO); // Bind pen VAO
       // glDrawElements(GL_TRIANGLES, penVerticesSize, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0); // Unbind VAO



        // Swap buffers and continue
        glfwSwapBuffers(window);
    }


    // Clean up
    glDeleteVertexArrays(1, &bookVAO);
    glDeleteBuffers(1, &bookVBO);
    glDeleteBuffers(1, &bookEBO);

    // Delete other VAOs, VBOs, textures, etc., for other objects

    glfwTerminate();
    return 0;
}