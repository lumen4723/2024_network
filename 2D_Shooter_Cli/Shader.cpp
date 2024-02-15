#include "Shader.h"

#include "lib.h"

Shader::Shader() {
    vertexShader = setup_shader(GL_VERTEX_SHADER, vertexShaderSource_Box);

    fragmentShader = setup_shader(GL_FRAGMENT_SHADER, fragmentShaderSource_Red);
    Red_shaderProgram = make_shader_program(vertexShader, fragmentShader);

    fragmentShader = setup_shader(GL_FRAGMENT_SHADER, fragmentShaderSource_Blue);
    Blue_shaderProgram = make_shader_program(vertexShader, fragmentShader);

    fragmentShader = setup_shader(GL_FRAGMENT_SHADER, fragmentShaderSource_Green);
    Green_shaderProgram = make_shader_program(vertexShader, fragmentShader);

    fragmentShader = setup_shader(GL_FRAGMENT_SHADER, fragmentShaderSource_Yellow);
    Yellow_shaderProgram = make_shader_program(vertexShader, fragmentShader);

    fragmentShader = setup_shader(GL_FRAGMENT_SHADER, fragmentShaderSource_Purple);
    Purple_shaderProgram = make_shader_program(vertexShader, fragmentShader);

    fragmentShader = setup_shader(GL_FRAGMENT_SHADER, fragmentShaderSource_Cyan);
    Cyan_shaderProgram = make_shader_program(vertexShader, fragmentShader);

    fragmentShader = setup_shader(GL_FRAGMENT_SHADER, fragmentShaderSource_White);
    White_shaderProgram = make_shader_program(vertexShader, fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(Red_shaderProgram);
    glDeleteProgram(Blue_shaderProgram);
    glDeleteProgram(Green_shaderProgram);
    glDeleteProgram(Yellow_shaderProgram);
    glDeleteProgram(Purple_shaderProgram);
    glDeleteProgram(Cyan_shaderProgram);
    glDeleteProgram(White_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

uint32_t Shader::get_shaderProgram(string color) {
    if (color == "Red") {
        return Red_shaderProgram;
    }
    else if (color == "Blue") {
        return Blue_shaderProgram;
    }
    else if (color == "Green") {
        return Green_shaderProgram;
    }
    else if (color == "Yellow") {
        return Yellow_shaderProgram;
    }
    else if (color == "Purple") {
        return Purple_shaderProgram;
    }
    else if (color == "Cyan") {
        return Cyan_shaderProgram;
    }
    else if (color == "White") {
        return White_shaderProgram;
    }
    else {
        return 0;
    }
}



uint32_t Shader::setup_shader(uint32_t type, const GLchar* shaderSource) {
    uint32_t shader = glCreateShader(type);
    if (type == GL_VERTEX_SHADER) {
        glShaderSource(shader, 1, &shaderSource, NULL);
    }
    else if (type == GL_FRAGMENT_SHADER) {
        glShaderSource(shader, 1, &shaderSource, NULL);
    }
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    char infoLog[512];
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << endl;
    }

    return shader;
}

uint32_t Shader::make_shader_program(uint32_t vertexShader, uint32_t fragmentShader) {
    uint32_t shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    
    char infoLog[512];
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
