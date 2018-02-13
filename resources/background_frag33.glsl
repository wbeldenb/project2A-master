#version 330 core
uniform vec2 uWindowSize;

out vec3 color;

void main()
{
	color = vec3(0.0, 0.0, 0.0);
	vec2 center = uWindowSize / 2;

	float length = sqrt(pow(center.x-gl_FragCoord.x, 2) + pow(center.y-gl_FragCoord.y, 2));
	color.r = 0.75 - length / 1000;
	color.b = 0.75- length / 1000;
}
