#ifndef _PHYSICSOBJECT_H_
#define _PHYSICSOBJECT_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <stdio.h>

class PhysicsObject{
public:
	PhysicsObject(int t) : type(t) { isPlayer = false; }
	PhysicsObject(GLuint shader, GLuint bordShader, int t, std::vector<std::vector<float>> terrain);
	PhysicsObject(const char *filepath, GLuint shader, GLuint bordShader, int t, int player);
	~PhysicsObject();

	//Functions
	void parse(const char *filepath);
	void render(glm::vec3 rgb);
	void render();
	void draw(glm::mat4 C);

	//Getters
	std::vector<GLuint> getIndices() { return indices; }
	std::vector<glm::vec3> getNormals() { return normals; }
	std::vector<glm::vec3> getVertices() { return vertices; }
	glm::vec3 getHalfExtents() { return hExtents; }
	glm::vec3 getPosition() { return glm::vec3(M[3]); }
	glm::mat4 getM() { return M; }
	bool getIsPlayer() { return isPlayer; }
	int getPlayer() { return player; }

	//Setters
	void setM(glm::mat4 m) { M = m; }
	void setPosition(glm::vec3 pos) { M[3] = glm::vec4(pos, 1.0f); }
	void scaleModel(glm::vec3 s) { M = M * glm::scale(glm::mat4(1.0f), s); }
	void setIsPlayer(bool b) { isPlayer = b; }
	void updateCamPos(glm::vec3 c) { campos = c; }
	void setScoreRadii(float r1, float r2, float r3);
	void setScoreCenters(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3);

private:
	glm::mat4 M = glm::mat4(1.0f);		//toWorld matrix
	glm::mat4 V = glm::mat4(1.0f);		//view matrix
	bool isPlayer;
	int player = -1;
	int type;

	float rad100 = 4.0f;
	float rad50 = 6.0f;
	float rad25 = 8.0f;
	glm::vec3 cent100 = glm::vec3(0.0f);
	glm::vec3 cent50 = glm::vec3(0.0f);
	glm::vec3 cent25 = glm::vec3(0.0f);

	//Parsing and Rendering
	GLuint VBO, VAO, EBO, VBO2;
	GLuint uProjection, uModel, uView, uCamPos, uCol, uOffset, uType, uCent100, uCent50, uCent25, uRad100, uRad50, uRad25;
	GLuint shaderProgram;
	GLuint borderShader;

	std::vector<GLuint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4 modelview;
	glm::vec3 campos;

	glm::vec3 hExtents;

	void initModel();
	void initHeightMapModel(std::vector<std::vector<float>> terrain);
};

#endif

