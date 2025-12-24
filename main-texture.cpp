#include "stb_image.h"

#define UTILS_MACRO_RENAME
#include "utils.hpp"

using namespace utils;

#include <math.h>
#include <stdio.h>

#include "graphics.hpp"

// os
static GLFWwindow *window;
static float current_time;

// gl buffers
static uint quad_vertex_buffer_object;  // vbo
static uint quad_vertex_array_object;   // vao
static uint quad_element_buffer_object; // ebo

static uint texture_vertex_buffer_object;  // vbo
static uint texture_vertex_array_object;   // vao
static uint texture_element_buffer_object; // ebo

// texture
static uint texture_map_wall;
static uint texture_map_happyface;

// shaders
static Shader quad_shader;
static Shader texture_shader;

// index

static float texture_vertices[] = {
    // pos ----------- | color ------------ | texture coord
    +0.5f, +0.5f, +0.0f, +0.5f, +0.5f, +0.5f, +1.0f, +1.0f, // top right
    +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +1.0f, +1.0f, +0.0f, // bottom right
    -0.5f, -0.5f, +0.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, // bottom left
    -0.5f, +0.5f, +0.0f, +1.0f, +0.3f, +1.0f, +0.0f, +1.0f, // top left
};

static uint texture_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3, // second triangle
};

static float quad_vertices[] = {
    // pos ---------- | color --------- |
    1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top right
    1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
    -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 0.0f, // top left
};

static uint quad_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3, // second triangle
};

// procs
static void update() {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    current_time = glfwGetTime();

    glfwPollEvents();
}

static void draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // // draw quad
    glBindVertexArray(quad_vertex_array_object);
    quad_shader.use();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    float f = min(abs(sin(current_time)) + 0.1f, 1.0f);
    quad_shader.set_vec4("global_color"_s, {f, f, f, 1.0f});

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // draw texture
    glBindVertexArray(texture_vertex_array_object);
    texture_shader.use();

    texture_shader.set_int("texture_map_wall"_s, 0);
    texture_shader.set_int("texture_map_happyface"_s, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_map_wall);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_map_happyface);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // end
    glfwSwapBuffers(window);
}

int main(void) {
    DEFER[] { glDeleteVertexArrays(1, &quad_vertex_array_object); };
    DEFER[] { glDeleteBuffers(1, &quad_vertex_buffer_object); };
    DEFER[] { glDeleteBuffers(1, &quad_element_buffer_object); };

    DEFER[] { glDeleteVertexArrays(1, &texture_vertex_array_object); };
    DEFER[] { glDeleteBuffers(1, &texture_vertex_buffer_object); };
    DEFER[] { glDeleteBuffers(1, &texture_element_buffer_object); };

    String_Builder sb;

    window = init("texture");
    DEFER[] { glfwTerminate(); };

    glViewport(0, 0, 640, 480);

    // create shaders program
    quad_shader = Shader("./assets/shaders/quad-vertex.glsl"_s,
                         "./assets/shaders/quad-fragment.glsl"_s, sb);

    texture_shader = Shader("./assets/shaders/texture-vertex.glsl"_s,
                            "./assets/shaders/texture-fragment.glsl"_s, sb);

    USE[] { // quad
        // create my buffers
        glGenBuffers(1, &quad_vertex_buffer_object);
        glGenVertexArrays(1, &quad_vertex_array_object);
        glGenBuffers(1, &quad_element_buffer_object);

        // bind my vertex array
        glBindVertexArray(quad_vertex_array_object);

        // fill my vertex buffer with shit
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices,
                     GL_STATIC_DRAW);

        // fill the ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_element_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices),
                     quad_indices, GL_STATIC_DRAW);

        // bind my quad_indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_element_buffer_object);

        // layout (location = 0) in vec3 pos;
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // layout (location = 1) in vec4 color;
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // unbind
        glBindVertexArray(0);
    };

    { // texture
        glGenVertexArrays(1, &texture_vertex_array_object);
        glGenBuffers(1, &texture_vertex_buffer_object);
        glGenBuffers(1, &texture_element_buffer_object);

        glBindVertexArray(texture_vertex_array_object);

        glBindBuffer(GL_ARRAY_BUFFER, texture_vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices),
                     texture_vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_element_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texture_indices),
                     texture_indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_element_buffer_object);

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

        // load texture wall
        glGenTextures(1, &texture_map_wall);
        glBindTexture(GL_TEXTURE_2D, texture_map_wall);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        USE[] {
            int width, height, nrChannels;
            u8 *data = stbi_load("./assets/img/container.jpg", &width, &height,
                                 &nrChannels, 0);
            assert(nrChannels == 3);
            assert(data);
            DEFER[data] { stbi_image_free(data); };

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        };

        glGenTextures(1, &texture_map_happyface);
        glBindTexture(GL_TEXTURE_2D, texture_map_happyface);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        USE[] {
            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true);
            u8 *data = stbi_load("./assets/img/awesomeface.png", &width,
                                 &height, &nrChannels, 0);
            assert(nrChannels == 4);
            assert(data);
            DEFER[data] { stbi_image_free(data); };

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        };

        // unbind
        glBindVertexArray(0);
    }

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
    }

    return 0;
}
