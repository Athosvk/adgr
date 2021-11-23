#version 330
uniform sampler2D c;
in vec2 uv;

out vec3 p;

void main()
{
	p=texture(c,uv).zyx;
}