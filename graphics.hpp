#pragma once

#include "utils.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

extern GLint max_vertex_attributes;

struct Shader {
    GLint id = -1;

    static constexpr utils::usize info_log_size = 512;

    explicit Shader();

    explicit Shader(const utils::String &vertex_shader_file_path,
                    const utils::String &fragment_shader_file_path,
                    utils::String_Builder<> &sb);

    static void compile(utils::uint shader, const utils ::String &file_path,
                        utils::String_Builder<> &sb);

    void use() const;

    GLint bind(const utils::String &name) const;

    void set_int(const utils::String &name, int n);
    void set_float_4(const utils::String &name, float x, float y, float z,
                     float w);
};

GLFWwindow *init(const char *title);
