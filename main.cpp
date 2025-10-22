#define UTILS_MACRO_RENAME
#include "utils.hpp"

using namespace utils;

#include <stdio.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

String_Builder<> sb;

GLFWwindow *window;

uint vertex_buffer_object;
uint vertex_array_object;

uint vertex_shader;
uint fragment_shader;
uint program_shader;

int success = 0;
char info_log[512] = {0};

float vertices[] = {
    -0.5f, -0.5f, 0.0f, // a
    0.5f,  -0.5f, 0.0f, // b
    0.0f,  0.5f,  0.0f, // c
};

void update() {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glfwPollEvents();
}

void draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
}

void shader_check(const char *name, const String &source, uint shader) {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("cant compile this stupid %s shader because: %s\n", name,
               info_log);
        exit(-1);
    }
}

String file_read(const char *file_path) {
    usize start = sb.length;
    FILE *file = fopen(file_path, "rb+");

    long tell_old = ftell(file);
    assert(tell_old >= 0);

    fseek(file, 0, SEEK_END);
    long tell_end = ftell(file);
    assert(tell_end >= 0);

    assert(tell_end >= tell_old);
    fseek(file, tell_old, SEEK_SET);

    long read_size = tell_end - tell_old;
    if (read_size <= 0)
        return String();

    sb.reserve(read_size);
    assert(fread(sb.items, 1, read_size, file) == read_size);
    sb.length += read_size;

    return String(sb.splice(start, -1));
}

int main(void) {
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

    glViewport(0, 0, 640, 480);
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

    // create my shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    DEFER[] { glDeleteShader(vertex_shader); };
    String vertex_shader_source = file_read("./vertex-shader.glsl");
    glShaderSource(
        vertex_shader, 1, &vertex_shader_source.items,
        reinterpret_cast<const GLint *>(&vertex_shader_source.length));
    glCompileShader(vertex_shader);

    shader_check("vertex", vertex_shader_source, vertex_shader);

    sb.length = 0;

    // create my shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    DEFER[] { glDeleteShader(fragment_shader); };
    String fragment_shader_source = file_read("./fragment-shader.glsl");
    glShaderSource(
        fragment_shader, 1, &fragment_shader_source.items,
        reinterpret_cast<const GLint *>(&fragment_shader_source.length));
    glCompileShader(fragment_shader);

    shader_check("frament", fragment_shader_source, fragment_shader);

    // create my program
    program_shader = glCreateProgram();
    glAttachShader(program_shader, vertex_shader);
    glAttachShader(program_shader, fragment_shader);
    glLinkProgram(program_shader);

    // cant link / create my program?
    glGetProgramiv(program_shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_shader, 512, NULL, info_log);
        printf("cant link this stupid program because: `%s`\n", info_log);
        return -1;
    }

    // bind my vertex array
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    // fill my vertex buffer with shit
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // bind my attributes
    // layout (location = 0) in vec3 aPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void *)0);
    glEnableVertexAttribArray(0);

    // use my shader
    glUseProgram(program_shader);

    // with my vao
    glBindVertexArray(vertex_array_object);

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
    }

    return 0;
}
