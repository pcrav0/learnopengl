#include "stb_image.h"

#define UTILS_MACRO_RENAME
#include "utils.hpp"

using namespace utils;

#include <math.h>
#include <stdio.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

// structs
struct Shader {
    GLint id = -1;

    static constexpr usize info_log_size = 512;

    explicit Shader() : id(-1) {}

    explicit Shader(const String &vertex_shader_file_path,
                    const String &fragment_shader_file_path,
                    String_Builder<> &sb) {
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
                   file_path.zstr(), info_log);
            exit(-1);
        }
    }

    void use() const { glUseProgram(id); }

    GLint bind(const String &name) const {
        GLint result = glGetUniformLocation(id, name.zstr());

        if (result < 0)
            printf("[-] cant find `%s` uniform, sorry :(\n", name.zstr());

        return result;
    }

    void set4f(const String &name, float x, float y, float z, float w) {
        glUniform4f(bind(name), x, y, z, w);
    }
};

// os
GLFWwindow *window;
float current_time;

// gl buffers
uint quad_vertex_buffer_object;   // vbo
uint quad_vertex_array_object;    // vao
uint quad_element_buffer_objects; // ebo

uint texture_vertex_buffer_object;   // vbo
uint texture_vertex_array_object;    // vao
uint texture_element_buffer_objects; // ebo

// shaders
Shader quad_shader;
Shader texture_shader;

// machine config

GLint max_vertex_attributes;
// index

float texture_vertices[] = {
    // pos ----------- | color ------------ | texture coord
    +0.5f, +0.5f, +0.0f, +0.5f, +0.5f, +0.5f, +1.0f, +1.0f, // top right
    +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +1.0f, +1.0f, +0.0f, // bottom right
    -0.5f, -0.5f, +0.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, // bottom left
    -0.5f, +0.5f, +0.0f, +1.0f, +0.3f, +1.0f, +0.0f, +1.0f, // top left
};

uint texture_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3, // second triangle
};

float quad_vertices[] = {
    // pos ---------- | color --------- |
    1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top right
    1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
    -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 0.0f, // top left
};

uint quad_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3, // second triangle
};

// texture
uint texture_map;

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

    // // draw quad
    glBindVertexArray(quad_vertex_array_object);
    quad_shader.use();

    // wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    float f = min(abs(sin(current_time)) + 0.1f, 1.0f);
    quad_shader.set4f("global_color"_s, f, f, f, 1.0f);

    // draw texture
    glBindVertexArray(texture_vertex_array_object);
    texture_shader.use();
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_map);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // end
    glfwSwapBuffers(window);
}

int texture_main(void) {
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

    // create shaders program
    quad_shader = Shader("./vertex-shader-quad.glsl"_s,
                         "./fragment-shader-quad.glsl"_s, sb);

    texture_shader = Shader("./vertex-shader-texture.glsl"_s,
                            "./fragment-shader-texture.glsl"_s, sb);

    { // quad
        // create my buffers
        glGenBuffers(1, &quad_vertex_buffer_object);
        glGenVertexArrays(1, &quad_vertex_array_object);
        glGenBuffers(1, &quad_element_buffer_objects);

        // bind my vertex array
        glBindVertexArray(quad_vertex_array_object);

        // fill my vertex buffer with shit
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices,
                     GL_STATIC_DRAW);

        // fill the ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_element_buffer_objects);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices),
                     quad_indices, GL_STATIC_DRAW);

        // bind my quad_indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_element_buffer_objects);

        // layout (location = 0) in vec3 pos;
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // layout (location = 1) in vec4 color;
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // unbind
    glBindVertexArray(0);

    { // texture
        glGenVertexArrays(1, &texture_vertex_array_object);
        glGenBuffers(1, &texture_vertex_buffer_object);
        glGenBuffers(1, &texture_element_buffer_objects);

        glBindVertexArray(texture_vertex_array_object);

        glBindBuffer(GL_ARRAY_BUFFER, texture_vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices),
                     texture_vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_element_buffer_objects);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texture_indices),
                     texture_indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_element_buffer_objects);

        // layout (location = 0) in vec3 pos;
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // layout (location = 1) in vec4 color;
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // layout (location = 2) in vec4 texture_cord;
        glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float),
                              (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // load texture
        glGenTextures(1, &texture_map);
        glBindTexture(GL_TEXTURE_2D, texture_map);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        {
            int width, height, nrChannels;
            u8 *data = stbi_load("./wall.jpg", &width, &height, &nrChannels, 0);
            assert(nrChannels == 3);
            assert(data);
            // DEFER[data = data] { stbi_image_free(data); };

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    // unbind
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
    }

    return 0;
}
