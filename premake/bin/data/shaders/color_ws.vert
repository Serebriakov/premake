#version 100

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform vec4 u_Color;

attribute vec2 a_Position;

void main()
{
	gl_Position = u_Projection * u_View * vec4(a_Position, 0, 1);
}

