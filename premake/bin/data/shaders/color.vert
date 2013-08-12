#version 100

uniform mat4 u_Projection;
uniform vec4 u_Color;

attribute vec2 a_Position;

void main()
{
	gl_Position = u_Projection * vec4(a_Position, 0, 1);
}

