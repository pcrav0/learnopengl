#define UTILS_MACRO_RENAME
#include "./graphics.hpp"

#include <unistd.h>

GLint max_vertex_attributes;

using utils::String;
using utils::String_Builder;

Shader::Shader() : id(-1) {}

Shader::Shader(const String &vertex_shader_file_path,
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

void Shader::compile(uint shader, const String &file_path,
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
               file_path.temp_zstr(), info_log);
        exit(-1);
    }
}

void Shader::use() { glUseProgram(id); }

GLint Shader::bind(const String &name) {
    GLint result = glGetUniformLocation(id, name.temp_zstr());

    if (result < 0)
        printf("[-] cant find `%s` uniform, sorry :(\n", name.temp_zstr());

    return result;
}

void Shader::set_int(const String &name, int n) { glUniform1i(bind(name), n); }

void Shader::set_vec4(const String &name, const glm::vec4 &vector) {
    glUniform4f(bind(name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::set_mat4(const utils::String &name, const glm::mat4 &matrix,
                      bool transpose) {
    glUniformMatrix4fv(bind(name), 1, transpose, glm::value_ptr(matrix));
}

GLFWwindow *init(const char *title) {
    printf("pid: %d\n", getpid());

    if (!glfwInit())
        exit(-1);

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

    GLFWwindow *result = glfwCreateWindow(640, 480, title, NULL, NULL);

    if (!result)
        exit(-1);

    glfwMakeContextCurrent(result);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        exit(-1);
    }

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);
    printf("[*] max vertex attributes = %d\n", max_vertex_attributes);

    glfwSetFramebufferSizeCallback(
        result, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

    glEnable(GL_DEPTH_TEST);

    return result;
}
