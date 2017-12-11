#version 330 core

layout (location = 0) in vec3 point;
layout (location = 1) in vec3 normals;

uniform mat4 model;
uniform mat4 projection;
uniform vec3 camPos;

out float norm;

void main(){
    gl_Position = projection * model * vec4(point, 1.0);
	norm = max(0.0, dot(normals, camPos));
}