#pragma once

#include "lib.h"

class Shader {
public:
    const GLchar* vertexShaderSource_Box = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";


    const GLchar* fragmentShaderSource_Red = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.2f, 0.2f, 1.0f);\n"
        "}\n\0";
    
    const GLchar* fragmentShaderSource_Blue = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.2f, 0.2f, 1.0f, 1.0f);\n"
        "}\n\0";
    
    const GLchar* fragmentShaderSource_Green = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.2f, 1.0f, 0.2f, 1.0f);\n"
        "}\n\0";
    
    const GLchar* fragmentShaderSource_Yellow = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
        "}\n\0";
    
    const GLchar* fragmentShaderSource_Purple = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.2f, 1.0f, 1.0f);\n"
        "}\n\0";
    
    const GLchar* fragmentShaderSource_Cyan = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.2f, 1.0f, 1.0f, 1.0f);\n"
        "}\n\0";

    const GLchar* fragmentShaderSource_White = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "}\n\0";


    uint32_t vertexShader;
    uint32_t fragmentShader;

    uint32_t Red_shaderProgram;
    uint32_t Blue_shaderProgram;
    uint32_t Green_shaderProgram;
    uint32_t Yellow_shaderProgram;
    uint32_t Purple_shaderProgram;
    uint32_t Cyan_shaderProgram;
    uint32_t White_shaderProgram;


    Shader();
    ~Shader();

    uint32_t setup_shader(uint32_t type, const GLchar* shaderSource);
    uint32_t make_shader_program(uint32_t vertexShader, uint32_t fragmentShader);

    uint32_t get_shaderProgram(string color);
};
