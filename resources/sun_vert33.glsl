#version 330 core
layout(location = 0) in vec3 vertPos;
uniform mat4 P;
uniform mat4 MV;
uniform float uTime;

void main()
{
	const float radius = 0.55;	

	vec3 position = vertPos;
	
	if (gl_VertexID % 2 == 0)
		position.xy *= abs(cos(uTime/3))+radius;

	else
		position.xy *= abs(sin(uTime/3))+radius;

	gl_Position = P * MV * vec4(position, 1.0);
}
