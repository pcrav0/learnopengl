#version 330 core
out vec4 result;

in vec3 fragment_color;
in vec2 fragment_texture_coord;

uniform sampler2D texture_map_wall;
uniform sampler2D texture_map_happyface;

void main()
{
    result = mix(
        texture(texture_map_wall, fragment_texture_coord),
        texture(texture_map_happyface, fragment_texture_coord), 0.2);

    result *= vec4(fragment_color, 1.0);
}
