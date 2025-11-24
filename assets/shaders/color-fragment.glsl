#version 330 core
out vec4 frag_result;

uniform vec3 object_color;
uniform vec3 light_color;
uniform float ambient_strength;

void main()
{
    vec3 ambient = ambient_strength * light_color;

    frag_result = vec4(ambient * object_color, 1.0);
}
