#version 330 core

in vec3 pos0;
in vec3 normal0;

out vec4 frag_result;

uniform mat4 model;
uniform float ambient_strength;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 object_color;

uniform bool has_ambient;
uniform bool has_diffuse;

void main() {
    vec3 ambient = ambient_strength * light_color;
    vec3 normal = mat3(transpose(inverse(model))) * normal0;
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos - pos0);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 result = vec3(0);

    if (has_ambient)
        result = result + ambient;

    if (has_diffuse)
        result = result + diffuse;

    frag_result = vec4(result * object_color, 1.0);
}
