#version 100

uniform mat4 u_Projection;

attribute vec2 a_Position;
attribute vec2 a_TextureCoordinate;

varying vec2 v_TextureCoordinate;

void main()
{
	v_TextureCoordinate = a_TextureCoordinate;
	gl_Position = u_Projection * vec4(a_Position, 0, 1);
}

