#include <stdio.h>

#define UTILS_MACRO_RENAME
#include "utils.hpp"

using namespace utils;

#include "graphics.hpp"

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

const static glm::vec3 color_white(1.0f, 1.0f, 1.0f);
const static glm::vec3 color_coral(1.0f, 0.5f, 0.31f);

static GLFWwindow *window;
static float current_time, delta_time;
static float acc = 0.0f, vel = 0.0f;
static GLuint vertex_buffer_object, vertex_array_object;
static Shader shader_obj, shader_light;

static glm::vec3 light_color(1.0f, 1.0f, 1.0f);
static glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

static struct {
    glm::vec3 pos, front, up;
    float yaw = -90.0f, pitch = 0.0f, fov = 45.0f;
} camera;

static bool in_menu = false;
static bool has_ambient = true, has_diffuse = true;

static glm::vec3 vertices[][2] = {
    {{-0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}},
    {{+0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}},
    {{+0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}},
    {{+0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}},
    {{-0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, -1.0f}},

    {{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}},
    {{+0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}},
    {{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}},
    {{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}},
    {{-0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}},
    {{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, +1.0f}},

    {{-0.5f, +0.5f, +0.5f}, {-1.0, +0.0f, +0.0f}},
    {{-0.5f, +0.5f, -0.5f}, {-1.0, +0.0f, +0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0, +0.0f, +0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0, +0.0f, +0.0f}},
    {{-0.5f, -0.5f, +0.5f}, {-1.0, +0.0f, +0.0f}},
    {{-0.5f, +0.5f, +0.5f}, {-1.0, +0.0f, +0.0f}},

    {{+0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}},
    {{+0.5f, +0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}},
    {{+0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}},
    {{+0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}},
    {{+0.5f, -0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}},
    {{+0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}},

    {{-0.5f, -0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}},
    {{+0.5f, -0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}},
    {{+0.5f, -0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}},
    {{+0.5f, -0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}},
    {{-0.5f, -0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}},

    {{-0.5f, +0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}},
    {{+0.5f, +0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}},
    {{+0.5f, +0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}},
    {{+0.5f, +0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}},
    {{-0.5f, +0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}},
    {{-0.5f, +0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}},
};

static bool update() {
    static float last_time;

    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }

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

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        light_pos.x -= camera_speed;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        light_pos.x += camera_speed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        light_pos.z -= camera_speed;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        light_pos.z += camera_speed;

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        glfwWaitEventsTimeout(1.5f);
        static int i = 0;
        printf("f2: %d\n", i++);
        in_menu = !in_menu;
    }

    glfwSetInputMode(window, GLFW_CURSOR,
                     in_menu ? GLFW_CURSOR_CAPTURED : GLFW_CURSOR_DISABLED);

    return true;
}

static void draw() {
    DEFER[] { glfwSwapBuffers(window); };

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vertex_array_object);
    shader_obj.use();

    static float ambient_strength = 1.0f;

    glm::mat4 view =
        glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 model = glm::mat4(1.0f);

    shader_obj.set_bool("has_ambient"_s, has_ambient);
    shader_obj.set_bool("has_diffuse"_s, has_diffuse);

    shader_obj.set_mat4("model"_s, model);
    shader_obj.set_mat4("projection"_s, projection);
    shader_obj.set_mat4("view"_s, view);

    shader_obj.set_vec3("light_color"_s, light_color);
    shader_obj.set_vec3("light_pos"_s, light_pos);
    shader_obj.set_vec3("object_color"_s, color_coral);
    shader_obj.set_float("ambient_strength"_s, ambient_strength);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    shader_light.use();
    model = glm::translate(model, light_pos);
    model = glm::scale(model, glm::vec3(0.2f));

    shader_light.set_mat4("model"_s, model);
    shader_light.set_mat4("projection"_s, projection);
    shader_light.set_mat4("view"_s, view);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    { // imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDebugLogWindow();
        ImGui::ShowDemoWindow();

        ImGui::Begin("config");
        ImGui::ColorEdit3("light color", (float *)&light_color);
        ImGui::SliderFloat("ambient strength light", &ambient_strength, 0.0f,
                           2.0f);
        ImGui::Selectable("has ambient light", &has_ambient);
        ImGui::Selectable("has diffuse light", &has_diffuse);
        ImGui::End();

        ImGui::EndFrame();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

int main() {
    String_Builder<> sb;

    window = init("color");
    DEFER[] { glfwTerminate(); };

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    shader_obj = Shader("./assets/shaders/color-vertex.glsl"_s,
                        "./assets/shaders/color-fragment.glsl"_s, sb);

    shader_light = Shader("./assets/shaders/color-vertex.glsl"_s,
                          "./assets/shaders/color-light-fragment.glsl"_s, sb);

    {
        glGenBuffers(1, &vertex_buffer_object);
        glGenVertexArrays(1, &vertex_array_object);

        glBindVertexArray(vertex_array_object);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);

        // layout (location = 0) in vec3 pos;
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertices[0]),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // layout (location = 1) in vec3 normal;
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertices[0]),
                              (void *)sizeof(glm::vec3));
        glEnableVertexAttribArray(1);
    }

    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos,
                                        double ypos) {
        if (in_menu) {
            ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
            return;
        }

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
            if (in_menu) {
                ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
                return;
            }

            camera.fov = clamp(camera.fov - (float)yoffset, 1.0f, 45.0f);
        });

    camera.pos = glm::vec3(0.0f, 0.0f, 3.0f);
    camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        if (!update())
            continue;
        draw();
    }

    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    return 0;
}
