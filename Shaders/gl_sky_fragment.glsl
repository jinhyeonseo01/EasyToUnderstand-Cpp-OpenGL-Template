#version 330 core

out vec4 color;
in vec2 uv0CS;

uniform sampler2D _MainTex;

void main ()
{
	color = vec4(texture(_MainTex, uv0CS).xyz, 1);
};