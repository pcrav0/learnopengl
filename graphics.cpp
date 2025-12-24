#define UTILS_MACRO_RENAME
#include "./graphics.hpp"

#include <unistd.h>

static ImGuiIO *imgui_io = nullptr;

GLint max_vertex_attributes;

using utils::String;
using utils::String_Builder;

Shader::Shader() : id(-1) {}

Shader::Shader(const String &vertex_shader_file_path,
               const String &fragment_shader_file_path, String_Builder &sb) {
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

void Shader::compile(uint shader, const String &file_path, String_Builder &sb) {
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

void Shader::set_bool(const String &name, bool b) {
    glUniform1i(bind(name), b);
}

void Shader::set_int(const String &name, int n) { glUniform1i(bind(name), n); }

void Shader::set_float(const String &name, float n) {
    glUniform1f(bind(name), n);
}

void Shader::set_vec3(const String &name, const glm::vec3 &vector) {
    glUniform3f(bind(name), vector.x, vector.y, vector.z);
}

void Shader::set_vec4(const String &name, const glm::vec4 &vector) {
    glUniform4f(bind(name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::set_mat4(const utils::String &name, const glm::mat4 &matrix,
                      bool transpose) {
    glUniformMatrix4fv(bind(name), 1, transpose, glm::value_ptr(matrix));
}

GLFWwindow *init(const char *title, int width, int height) {
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

#if DEBUG
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "#DEBUG");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "#DEBUG");
#endif

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(
        glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow *result = glfwCreateWindow(
        main_scale * width, main_scale * height, title, NULL, NULL);

    if (!result)
        exit(-1);

    glfwMakeContextCurrent(result);
    glfwSwapInterval(1);

    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imgui_io = &ImGui::GetIO();
        imgui_io->ConfigFlags |=
            ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        imgui_io->ConfigFlags |=
            ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes(
            main_scale); // Bake a fixed style scale. (until we have a solution
                         // for dynamic style scaling, changing this requires
                         // resetting Style + calling this again)
        style.FontScaleDpi =
            main_scale; // Set initial font scale. (using
                        // io.ConfigDpiScaleFonts=true makes this unnecessary.
                        // We leave both here for documentation purpose)
    }
    ImGui_ImplOpenGL3_Init("#version 330 core");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        exit(-1);
    }

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);
    printf("[*] max vertex attributes = %d\n", max_vertex_attributes);

    glfwSetFramebufferSizeCallback(
        result, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

    assert(DEBUG);

#if DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
           GLsizei length, const GLchar *message, const void *userParam) {
            const char *_source;
            const char *_type;
            const char *_severity;

            switch (source) {
            case GL_DEBUG_SOURCE_API:
                _source = "API";
                break;

            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                _source = "WINDOW SYSTEM";
                break;

            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                _source = "SHADER COMPILER";
                break;

            case GL_DEBUG_SOURCE_THIRD_PARTY:
                _source = "THIRD PARTY";
                break;

            case GL_DEBUG_SOURCE_APPLICATION:
                _source = "APPLICATION";
                break;

            case GL_DEBUG_SOURCE_OTHER:
                _source = "UNKNOWN";
                break;

            default:
                _source = "UNKNOWN";
                break;
            }

            switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                _type = "ERROR";
                break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                _type = "DEPRECATED BEHAVIOR";
                break;

            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                _type = "UDEFINED BEHAVIOR";
                break;

            case GL_DEBUG_TYPE_PORTABILITY:
                _type = "PORTABILITY";
                break;

            case GL_DEBUG_TYPE_PERFORMANCE:
                _type = "PERFORMANCE";
                break;

            case GL_DEBUG_TYPE_OTHER:
                _type = "OTHER";
                break;

            case GL_DEBUG_TYPE_MARKER:
                _type = "MARKER";
                break;

            default:
                _type = "UNKNOWN";
                break;
            }

            switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                _severity = "HIGH";
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                _severity = "MEDIUM";
                break;

            case GL_DEBUG_SEVERITY_LOW:
                _severity = "LOW";
                break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
                _severity = "NOTIFICATION";
                break;

            default:
                _severity = "UNKNOWN";
                break;
            }

            printf("[GLERROR, source: %s, type: %s, severity: %s]:\n%.*s\n",
                   _source, _type, _severity, length, message);

            if (severity == GL_DEBUG_SEVERITY_HIGH)
                exit(-1);
        },
        nullptr);
#endif

    return result;
}
