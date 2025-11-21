#pragma once

#include "utils.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

extern GLint max_vertex_attributes;

struct Shader {
    GLint id = -1;

    static constexpr utils::usize info_log_size = 512;

    explicit Shader();

    explicit Shader(const utils::String &vertex_shader_file_path,
                    const utils::String &fragment_shader_file_path,
                    utils::String_Builder<> &sb);

    static void compile(utils::uint shader, const utils::String &file_path,
                        utils::String_Builder<> &sb);

    void use();

    GLint bind(const utils::String &name);

    void set_int(const utils::String &name, int n);
    void set_vec4(const utils::String &name, const glm::vec4 &vector);
    void set_mat4(const utils::String &name, const glm::mat4 &matrix,
                  bool transpose = false);
};

// static struct {
//     glm::vec3 pos, front, up;
//     float yaw = -90.0f, pitch = 0.0f, fov = 45.0f;
// } camera;

struct Texture {
    GLuint id = -1;
};

GLFWwindow *init(const char *title);

struct DrawOptions {
    Texture texture;
    glm::vec4 color = glm::vec4(1.0f);
    glm::vec3 rotation = glm::vec3(1.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 translate = glm::vec3(0);
};

void triagle_draw(glm::vec2 pos, DrawOptions opts = {});
void quad_draw(glm::vec2 pos, DrawOptions opts = {});
void cube_draw(glm::vec3 pos, DrawOptions opts = {});
