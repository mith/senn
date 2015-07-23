#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>

void gl_check_error(int line);
GLuint create_shader(GLenum shader_type, const std::string& shader_src);
GLuint create_program(const std::vector<GLuint>& shader_list);
