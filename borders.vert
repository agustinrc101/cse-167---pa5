#version 330 core

layout (location = 0) in vec3 point;
layout (location = 1) in vec3 normals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float offset;

out vec4 pointNormPos;

void main(){
	vec4 worldPos = model * vec4(point, 1.0f);
	vec4 pointPos = projection * view * worldPos;
	vec4 pointNor = projection * view * model * vec4(normals, 1.0f);
	pointNormPos = pointPos + (0.03 * pointNor);

	gl_Position = pointPos;
}