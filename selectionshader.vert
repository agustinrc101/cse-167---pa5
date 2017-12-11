#version 330 core

layout (location = 0) in vec3 point;

uniform mat4 projection;
uniform mat4 modelview;

void main(){
	gl_Position = projection * modelview * vec4(point, 1.0);
}