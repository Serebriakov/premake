#version 100

uniform sampler2D u_Texture;
varying vec2 v_TextureCoordinate;


void main()
{
	gl_FragColor = texture2D(u_Texture, v_TextureCoordinate);
}
