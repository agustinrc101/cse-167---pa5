#version 330 core

uniform uint id;
uniform int mode;

out vec4 finalColor;

void main(){
	finalColor = vec4((256.0 - id)/255.0);
}