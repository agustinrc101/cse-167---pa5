#version 330 core
out vec4 finalColor;

in vec3 normal;
in vec3 lightDir;
in vec3 camDir;
in vec3 pt;

uniform vec3 rgb;
uniform int type;

uniform vec3 center100;
uniform vec3 center50;
uniform vec3 center25;
uniform float radius100;
uniform float radius50;
uniform float radius25;

float diffuseSimple(vec3 L, vec3 N){
	return clamp(dot(L, N), 0.0, 1.0);
}

float specularSimple(vec3 L, vec3 N, vec3 H){
	if(dot(N, L) < 0.0){
		return pow(clamp(dot(H, N), 0.0, 1.0), 64);
	}
	return 0.0;
}

void main(){
	vec3 coloring;

	if(type == 0){
		coloring = 	rgb;
	}
	else{
		if     (distance(pt, center100) - radius100/2.0 < radius100 / 2.0)
			coloring = vec3(1.0, 0.0, 117.0 / 255.0);
		else if(distance(pt, center50 ) - radius50/2.0  < radius50 / 2.0)
			coloring = vec3(1.0, 125.0 / 255.0, 0.0);
		else if(distance(pt, center25 ) - radius25/2.0   < radius25 / 2.0)
			coloring = vec3(0.0, 1.0, 1.0);
		else if(distance(pt, vec3(0,0,0)) - 25.0     < 25.0)
			coloring = vec3(1.0, 1.0, 159.0 / 255.0);
		else
			coloring = rgb;
	}
	
	vec3 halfVector = normalize(lightDir + camDir);
	float amb = 0.1;
	float diff = diffuseSimple(lightDir, normal);
	float spec = specularSimple(lightDir, normal, halfVector);
	float intensity = amb + diff + spec;
	float shadeIntensity = ceil(intensity * 5.0) / 5.0;


	

	finalColor = vec4(coloring * shadeIntensity, 1.0);
		
/*
	vec3 camDirection = normalize(cam - pt);
	vec3 lightDirection = normalize(lightPos - pt);
	vec3 normalDirection = normalize(normal - pt);

	float camIntensity = max(0.0, dot(camDirection, normalDirection));
	float lightIntensity = max(0.0, dot(lightDirection, normalDirection));

	vec4 color;

	//Edge Shading
	float edge = camIntensity < 0.5 ? 0.75 : 1.0;	//If camIntensity < 0.5, set to 0.75, else set to 1.0;
	edge = camIntensity < 0.25 ? 0.5 : edge;		//If camIntensity < 0.25, set to 0.5, else dont change;
	edge = camIntensity < 0.05 ? 0.25 : edge;		//If camIntensity < 0.05, set to 0.25, else dont change;

	//Other types of shading
	float edgeL = lightIntensity < 0.5 ? 0.75 : 1.0;
	edgeL = lightIntensity < 0.05 ? 0.25 : edgeL;

	color = vec4(edge) * 0.5 + vec4(edgeL) * 0.5;
	color = vec4(color.x, color.y, color.z, 0.0);

	color = vec4(color.x * coloring.x, color.y * coloring.y, color.z * coloring.z, 0.0);

	finalColor = color;
*/
}