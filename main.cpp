#define UTILS_MACRO_RENAME
#include "utils.hpp"

using namespace utils;

#include <math.h>
#include <stdio.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

// os
GLFWwindow *window;
float current_time;

// gl buffers
uint vertex_buffer_object;   // vbo
uint vertex_array_object;    // vao
uint element_buffer_objects; // ebo

// shaders
uint program_shader;

// machine config

GLint max_vertex_attributes;

// index

float vertices[] = {
    // pos -----------| color
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, // top left
};

uint indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

// uniforms
int global_color;

// static buffers
static constexpr usize uniform_name_size = 512;
static char uniform_name[uniform_name_size + 1];

// structs
struct Shader {
    uint id;

    static constexpr usize info_log_size = 512;

    static void compile(uint shader, const String &file_path,
                        String_Builder<> &sb) {
        String source = file_read(file_path, sb);

        glShaderSource(shader, 1, &source.items,
                       reinterpret_cast<const GLint *>(&source.length));
        glCompileShader(shader);

        int success;
        char info_log[info_log_size] = {0};

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, info_log);
            printf("cant compile this stupid %s shader because: %s\n",
                   file_path, info_log);
            exit(-1);
        }
    }

    Shader(const String &vertex_shader_file_path,
           const String &fragment_shader_file_path, String_Builder<> &sb) {
        uint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        DEFER[=] { glDeleteShader(vertex_shader); };
        compile(vertex_shader, vertex_shader_file_path, sb);

        uint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        DEFER[=] { glDeleteShader(fragment_shader); };
        compile(fragment_shader, fragment_shader_file_path, sb);

        id = glCreateProgram();
        glAttachShader(id, vertex_shader);
        glAttachShader(id, fragment_shader);
        glLinkProgram(id);

        int success;
        char info_log[info_log_size] = {0};
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 512, NULL, info_log);
            printf("[-] cand link the shader program because: %s\n", info_log);
            exit(-1);
        }
    }

    void use() const { glUseProgram(id); }

    GLint bind(const String &name) const {
        assert(name.length < uniform_name_size);
        memset(uniform_name, 0, uniform_name_size);
        memcpy(uniform_name, name.items, name.length);

        GLint result = glGetUniformLocation(id, uniform_name);
        if (result < 0)
            printf("[-] cant find `global_color` uniform, sorry :(\n");

        return result;
    }
};

// procs
void update() {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    current_time = glfwGetTime();

    glfwPollEvents();
}

void draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, ARRAY_LENGTH(indices), GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    float f = min(abs(sin(current_time)) + 0.1f, 1.0f);
    glUniform4f(global_color, f, f, f, 1.0f);

    glfwSwapBuffers(window);
}

int main(void) {
    String_Builder<> sb;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CENTER_CURSOR, true);

    glfwWindowHint(GLFW_CONTEXT_DEBUG, DEBUG);
    glfwWindowHint(GLFW_RESIZABLE, DEBUG);
    glfwWindowHint(GLFW_FLOATING, DEBUG);
    glfwWindowHint(GLFW_MAXIMIZED, !DEBUG);

#ifdef DEBUG
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "#DEBUG");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "#DEBUG");
#endif

    window = glfwCreateWindow(640, 480, "learnopengl.com", NULL, NULL);
    DEFER[] { glfwTerminate(); };

    if (!window) {
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);
    printf("[*] max vertex attributes = %d\n", max_vertex_attributes);

    glViewport(0, 0, 640, 480);
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

    // create my program
    auto program_shader =
        Shader("./vertex-shader.glsl"_s, "./fragment-shader.glsl"_s, sb);

    // bind uniforms
    global_color = program_shader.bind("global_color"_s);

    // bind my vertex array
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    // fill my vertex buffer with shit
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // fill the ebo
    glGenBuffers(1, &element_buffer_objects);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_objects);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // bind my indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_objects);

    // bind my attributes

    // layout (location = 0) in vec3 aPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, false,
                          ARRAY_LENGTH(indices) * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // layout (location = 1) in vec4 color;
    glVertexAttribPointer(1, 3, GL_FLOAT, false,
                          ARRAY_LENGTH(indices) * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // use my shader
    program_shader.use();

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
    }

    return 0;
}
