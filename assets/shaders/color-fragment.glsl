#version 330 core

in vec3 pos0;
in vec3 normal0;

out vec4 frag_result;

uniform mat4 model;
uniform vec3 camera_pos;

uniform int specular_n;

uniform float specular_strength;
uniform float ambient_strength;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 object_color;

uniform bool has_ambient;
uniform bool has_diffuse;
uniform bool has_specular;

void main() {
    vec3 result = vec3(0);

    vec3 normal = mat3(transpose(inverse(model))) * normal0;
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos - pos0);

    if (has_ambient) {
        vec3 ambient = ambient_strength * light_color;
        result = result + ambient;
    }

    if (has_diffuse) {
        float diff = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = diff * light_color;
        result = result + diffuse;
    }

    if (has_specular) {
        vec3 camera_dir = normalize(camera_pos);
        vec3 reflect_dir = reflect(-light_dir, norm);
        float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), 1 << specular_n);
        vec3 specular = specular_strength * spec * light_color;
        result = result + specular;
    }

    frag_result = vec4(result * object_color, 1.0);
}
