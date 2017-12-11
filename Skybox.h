#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <float.h>
#include "shader.h"

class Skybox{
	GLint shader;

public:
	Skybox(GLint shad);
	Skybox(GLint shad, float p);
	~Skybox();

	unsigned int getTextureID();
	GLuint getCubeMapVAO();
	
	void draw(GLint shader);
	void initVertices(float p);

private:
	unsigned int textureID;
	std::vector<glm::vec3> vertices;

	GLuint VBO, VAO, EBO, VBO2;
	GLuint uProjection, uModelview;

	void loadCubeMap(std::vector<std::string> faces);
	
	void initCubeMap();
	void helperPrint(int i);

};

#endif