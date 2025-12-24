#define UTILS_MACRO_RENAME
#include "utils.hpp"

#include "stb_image.h"

#include <math.h>
#include <stdio.h>

#include "graphics.hpp"

using namespace utils;

GLFWwindow *window;
float current_time;

// object

uint vertex_buffer_object;
uint vertex_array_object;
uint element_buffer_object;

Array<uint> textures;

Shader shader;

struct Vertice {
    glm::vec3 pos;
    glm::vec2 texture_cord;
};

Vertice vertices[] = {
    // top right
    {
        .pos = {+0.5f, +0.5f, +0.0f},
        .texture_cord = {+1.0f, +1.0f},
    },

    // bottom right
    {
        .pos = {+0.5f, -0.5f, +0.0f},
        .texture_cord = {+1.0f, +0.0f},
    },

    // bottom left
    {
        .pos = {-0.5f, -0.5f, +0.0f},
        .texture_cord = {+0.0f, +0.0f},
    },

    // top left
    {
        .pos = {-0.5f, +0.5f, +0.0f},
        .texture_cord = {+0.0f, +1.0f},
    },
};

uint indices[][3] = {
    {0, 1, 3},
    {1, 2, 3},
};

void load_texture(const char *file_path, bool flip = false) {
    uint texture;

    // load texture wall
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textures.push(texture);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip);
    u8 *data = stbi_load(file_path, &width, &height, &channels, 0);
    assert(data);
    DEFER[data = data] { stbi_image_free(data); };

    uint format;
    switch (channels) {
    case 1:
        format = GL_RED;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        assert(false);
        break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void update() {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    current_time = glfwGetTime();

    glfwPollEvents();
}

void draw() {
    auto draw_allocator = Allocator_Ring<1024>();
    context_allocator_push(&draw_allocator);
    DEFER[] { context_allocator_destroy(); };

    glClearColor((float)0x18 / 0xff, (float)0x18 / 0xff, (float)0x18 / 0xff,
                 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vertex_array_object);
    shader.use();

    assert(textures.length <= 16);
    for (uint i = 0; i < textures.length; i++) {
        String_Builder sb;
        sb.start();
        sb.write("texture_"_s);
        sb.write('0' + i);
        shader.set_int(sb.end(), i);
        sb.reset();

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }

    glm::mat4 m(1.0f);
    m = glm::scale(m, glm::vec3(0.5, 0.5, 0.5));
    // m = glm::translate(m, glm::vec3(0.5f, 0.0f, 0.5f));
    m = glm::rotate(m, current_time, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.set_mat4("transform"_s, m);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}

int main(void) {
    String_Builder sb;

    window = init("transformation");
    DEFER[] { glfwTerminate(); };

    glViewport(0, 0, 640, 480);

    shader = Shader("./assets/shaders/cube-vertex.glsl"_s,
                    "./assets/shaders/cube-fragment.glsl"_s, sb);

    glGenBuffers(1, &vertex_buffer_object);
    glGenVertexArrays(1, &vertex_array_object);
    glGenBuffers(1, &element_buffer_object);

    glBindVertexArray(vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);

    // layout (location = 0) in vec3 pos;
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertice),
                          (void *)offsetof(Vertice, pos));
    glEnableVertexAttribArray(0);

    // layout (location = 1) in vec2 texture_coord;
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertice),
                          (void *)offsetof(Vertice, texture_cord));
    glEnableVertexAttribArray(1);

    load_texture("./assets/img/wall.jpg", true);
    load_texture("./assets/img/awesomeface.png", true);
    assert(textures.length == 2);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
        context_allocator_reset();
    }

    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteBuffers(1, &vertex_buffer_object);
    glDeleteBuffers(1, &element_buffer_object);
    glDeleteProgram(shader.id);

    sb.reset();

    return 0;
}
