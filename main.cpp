#define UTILS_MACRO_RENAME
#include "utils.hpp"

using namespace utils;

#include <math.h>
#include <stdio.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

// memory
String_Builder<> sb;

// os
GLFWwindow *window;
float current_time;

// gl buffers
uint vertex_buffer_object;   // vbo
uint vertex_array_object;    // vao
uint element_buffer_objects; // ebo

// shaders
uint vertex_shader;
uint fragment_shader;
uint program_shader;

// machine config

GLint max_vertex_attributes;

// error setup
int success = 0;
char info_log[512] = {0};

// index

float vertices[] = {
    // pos -----------| color
    // 0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // top right
    // 0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    //-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
    //-0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, // top left

    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
};

uint indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

// uniforms
int global_color;

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

    // glDrawElements(GL_TRIANGLES, ARRAY_LENGTH(indices), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    float f = min(abs(sin(current_time)) + 0.1f, 1.0f);
    glUniform4f(global_color, f, f, f, 1.0f);

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

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);
    printf("[*] max vertex attributes = %d\n", max_vertex_attributes);

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

    // bind uniforms
    global_color = glGetUniformLocation(program_shader, "global_color");
    if (global_color < 0) {
        printf("[-] cant find `global_color` uniform, sorry :(\n");
        // return -1;
    }

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
    glUseProgram(program_shader);

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
    }

    return 0;
}
