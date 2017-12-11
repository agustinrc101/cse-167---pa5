#version 330 core

in float norm;

out vec4 finalColor;

uniform vec3 rgb;

void main(){
	if(norm != 0.0){
		finalColor = vec4(rgb.x, rgb.y, rgb.z, 0);
	}
	else{
		finalColor = vec4(0, 0, 0, 0);
	}
}