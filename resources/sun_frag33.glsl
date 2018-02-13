#version 330 core

uniform vec2 uWindowSize;

out vec3 color;

void main() {
	color = vec3(1.0, 1.0, 0.0);
	vec2 center = uWindowSize / 2;
	float length = sqrt(pow(center.x-gl_FragCoord.x, 2) + pow(center.y-gl_FragCoord.y, 2));
	color.g = 1.0 - length / 500;

	if (length(gl_FragCoord.xy - uWindowSize / 2.0) < 25.0)
		color.r = color.g = 1.0;
}