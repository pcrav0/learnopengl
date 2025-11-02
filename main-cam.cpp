#define UTILS_MACRO_RENAME
#include "utils.hpp"

#include "stb_image.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

#include "graphics.hpp"

using namespace utils;

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

static GLFWwindow *window;
static float current_time;
static float delta_time;

// object

static uint vertex_buffer_object;
static uint vertex_array_object;
static uint element_buffer_object;

static Array<uint> textures;

static Shader shader;

struct Vertice {
    glm::vec3 pos;
    glm::vec2 texture_cord;
};

static Vertice vertices[] = {
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},

    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},

    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},

    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},

    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}},
};

glm::vec3 cubes_pos[10] = {
    {0.0f, 0.0f, 0.0f},     {2.0f, 5.0f, -15.0f}, {-1.5f, -2.2f, -2.5f},
    {-3.8f, -2.0f, -12.3f}, {2.4f, -0.4f, -3.5f}, {-1.7f, 3.0f, -7.5f},
    {1.3f, -2.0f, -2.5f},   {1.5f, 2.0f, -2.5f},  {1.5f, 0.2f, -1.5f},
    {-1.3f, 1.0f, -1.5f},
};

glm::vec4 cubes_rot[10] = {};

static uint indices[][3] = {
    // {0, 1, 3},
    // {1, 2, 3},
};

static struct {
    glm::vec3 pos, front, up;
    float yaw = -90.0f, pitch = 0.0f, fov = 45.0f;
} camera;

static float acc = 0.0f;
static float vel = 0.0f;

static f32 random_norm_neg() { return random_norm() * 2.0f - 1.0f; }

static void load_texture(const char *file_path, bool flip = false) {
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

static void update() {
    static float last_time;

    current_time = glfwGetTime();
    delta_time = current_time - last_time;
    last_time = current_time;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float camera_speed = 2.5f * delta_time;

    vel = clamp(vel + acc * delta_time, -100.0f, 100.0f);
    acc = clamp(acc - 1500.0f * delta_time, -10.0f, 100.0f);
    camera.pos.y = clamp(camera.pos.y + vel * delta_time, 0.0f, 10000.0f);

    if (camera.pos.y == 0) {
        vel = 0.0f;
        acc = 0.0f;
    }

    printf("%.2f, %.2f\n", acc, vel);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && camera.pos.y == 0) {
        acc = 100.0f;
        vel = 3.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.pos += camera_speed * camera.front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.pos -= camera_speed * camera.front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.pos -=
            glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.pos +=
            glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;

    glfwPollEvents();
}

static void draw() {
    static Allocator *temp_allocator =
        create_temp_allocator(Allocator_Ring<1024>());

    glClearColor((float)0x18 / 0xff, (float)0x18 / 0xff, (float)0x18 / 0xff,
                 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vertex_array_object);
    shader.use();

    assert(textures.length <= 16);
    for (uint i = 0; i < textures.length; i++) {
        String_Builder<temp_allocator> sb;
        sb.start();
        sb.write("texture_"_s);
        sb.write('0' + i);
        shader.set_int(sb.end(), i);
        sb.reset();

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    glm::mat4 view =
        glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader.set_mat4("projection"_s, projection);
    shader.set_mat4("view"_s, view);

    for (uint i = 0; i < ARRAY_LENGTH(cubes_pos); i++) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 pos = cubes_pos[i];
        glm::vec4 rot = cubes_rot[i];

        model = glm::translate(model, pos);
        float angle = rot.w * 100.0f;
        model = glm::rotate(model, current_time * glm::radians(angle),
                            glm::vec3(rot) * 2.0f);
        shader.set_mat4("model"_s, model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
}

int main(void) {
    String_Builder<> sb;

    window = init("cord");
    DEFER[] { glfwTerminate(); };

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    shader = Shader("./assets/shaders/cord-vertex.glsl"_s,
                    "./assets/shaders/cord-fragment.glsl"_s, sb);

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

    random_state = time(nullptr);
    for (uint i = 0; i < ARRAY_LENGTH(cubes_rot); i++) {
        cubes_rot[i] = glm::vec4(random_norm_neg(), random_norm_neg(),
                                 random_norm_neg(), random_norm_neg());
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos,
                                        double ypos) {
        static bool frist_input = true;
        static float last_x = 400.0f, last_y = 300.0f;

        if (frist_input) {
            last_x = xpos;
            last_y = ypos;
            frist_input = false;
        }

        float xoffset = xpos - last_x;
        float yoffset = last_y - ypos;
        last_x = xpos;
        last_y = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        camera.yaw += xoffset;
        camera.pitch = clamp(camera.pitch + yoffset, -89.0f, 89.0f);

        glm::vec3 direction(
            cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)),
            sin(glm::radians(camera.pitch)),
            sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)));
        camera.front = glm::normalize(direction);
    });

    glfwSetScrollCallback(
        window, [](GLFWwindow *window, double xoffset, double yoffset) {
            camera.fov = clamp(camera.fov - (float)yoffset, 1.0f, 45.0f);
        });

    camera.pos = glm::vec3(0.0f, 0.0f, 3.0f);
    camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

    update();

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
        global_allocator->reset();
    }

    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteBuffers(1, &vertex_buffer_object);
    glDeleteBuffers(1, &element_buffer_object);
    glDeleteProgram(shader.id);

    sb.reset();

    return 0;
}
