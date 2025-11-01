#version 330 core
out vec4 result;

in vec2 fragment_texture_coord;

uniform sampler2D texture_0;
uniform sampler2D texture_1;

void main()
{
    result = mix(
        texture(texture_0, fragment_texture_coord),
        texture(texture_1, fragment_texture_coord), 0.2);

    // result *= vec4(fragment_color, 1.0);
}
