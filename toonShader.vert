#version 330 core

layout (location = 0) in vec3 point;
layout (location = 1) in vec3 normals;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec3 camPos;

out vec3 normal;
out vec3 lightDir;
out vec3 camDir;
out vec3 pt;

void main(){
	vec4 worldPos = model * vec4(point, 1.0f);
	mat4 G = transpose(inverse(view * model));

	normal = normalize(normals * vec3(G * vec4(normals, 0.0)));	

	lightDir = normalize(vec3(100.0, 1000.0, 0.0) - vec3(worldPos.x, worldPos.y, worldPos.z));
	camDir = normalize(camPos - vec3(worldPos.x, worldPos.y, worldPos.z));

	pt = point;

/*	normal = normals;
	pt = point;
	cam = camPos;
*/  gl_Position = projection * view * worldPos;//projection * modelview * vec4(point, 1.0);
}