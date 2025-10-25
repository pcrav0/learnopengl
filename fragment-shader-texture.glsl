#version 330 core
out vec4 result;

in vec3 fragment_color;
in vec2 fragment_texture_coord;

uniform sampler2D texture_img;

void main()
{
    result = texture(texture_img, fragment_texture_coord) * vec4(fragment_color, 1.0);
}
