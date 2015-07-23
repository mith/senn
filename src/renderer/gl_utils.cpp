#include "gl_utils.hpp"
#include <stdexcept>

void gl_check_error(int line)
{
    GLenum errorcode = glGetError();
    switch (errorcode) {
    case GL_INVALID_ENUM:
        std::printf("function called with invalid enum at line %d\n", line);
        break;
    case GL_INVALID_VALUE:
        std::printf("argument has invalid value at line %d\n", line);
        break;
    case GL_INVALID_OPERATION:
        std::printf("operation is invalid at line %d\n", line);
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::printf("framebuffer object not complete at line %d\n", line);
        break;
    default:
        std::printf("no error detected\n");
        break;
    }
}

GLuint create_shader(GLenum shader_type, const std::string& shader_src)
{
    GLuint shader = glCreateShader(shader_type);
    const char* strFileData = shader_src.c_str();
    glShaderSource(shader, 1, &strFileData, nullptr);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if ((GLboolean)status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto strInfoLog = std::vector<GLchar>(infoLogLength + 1);
        glGetShaderInfoLog(shader, infoLogLength, nullptr, strInfoLog.data());
        throw std::runtime_error(std::string(strInfoLog.data()));
    }

    return shader;
}

GLuint create_program(const std::vector<GLuint>& shader_list)
{
    GLuint program = glCreateProgram();

    for (auto shader : shader_list) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if ((GLboolean)status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, nullptr, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (auto shader : shader_list) {
        glDetachShader(program, shader);
    }

    return program;
}
