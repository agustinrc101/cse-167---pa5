#ifndef _ISLANDSGL_H_
#define _ISLANDSGL_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "PhysicsEngine.h"

struct PointAreas {
	float radius = 0.0f;
	glm::vec3 position = glm::vec3(0.0f);
};

/* This Class will check if an object is inside */
class IslandsGL {
	PhysicsEngine * physicsEngine;


public:
	IslandsGL(float arenaRadius, glm::vec3 arenaCenter, GLuint toon, GLuint border) 
		: radius(arenaRadius), center(arenaCenter), cellShader(toon), borderShader(border) {
		score100 = new PointAreas();
		score50 = new PointAreas();
		score25 = new PointAreas();
		physicsEngine = new PhysicsEngine();
		loadScoreLocations();	//Initializes 3 scores locations (100, 50, 25)
		generateHeightField();
		initArena();
	}
	~IslandsGL();

	void draw(glm::mat4 C);
	void update(float delta);

	//getters
	glm::vec3 getScoreCenter(int score);
	glm::vec3 getNewCamPos();
	float getScoreRadius(int score);
	bool getIsPaused() { return paused; }
	bool getGameIsOver() { return gameIsOver; }
	bool getIsEvent1() { return event1; }
	bool getIsEvent2() { return event2; }
	bool getIsEvent3() { return event3; }

	//Setters
	void togglePause() { paused = !paused; }
	void setLaunched(bool b) { launched = b; }

	//Other helper functions
	void mouseMovement(float x, float y);
	void addHeightField(GLuint shader, GLuint bordShader, std::vector<std::vector<float>> field, int size, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));

private:
	GLuint cellShader, borderShader;
	
	glm::vec3 center;		//If we use a box, just do corner comparisons (make sure it's within xmax and xmin, and ymax and ymin)
	float radius;

	float xMoved = 0.0f;
	float yMoved = 0.0f;

	int p1Score = 0;
	int p2Score = 0;
	int p3Score = 0;
	int p4Score = 0;

	PointAreas * score100;
	PointAreas * score50;
	PointAreas * score25;

	const char * model = "./objects/sphere.obj";
	bool paused = false;
	bool event1 = false;	//Player aims
	bool event2 = false;	//Player shoots (follow thrown object)
	bool event3 = false;	//calculate scores, loop back
	bool launched = false;
	bool gameIsOver = false;//when curRound < total#of rounds
	bool canAddModels = false; //adds models, once per round
	int curRound = 0;
	int curPlayer = 0;
	float gameEllapsedTime = 0;
	float strength = 0.0f;
	glm::vec3 initialPos = glm::vec3(0.0f, 20.0f, -105.0f);
	//Every other point ON the platform is 10 pts, outside of under the platform (set threshold) is 0 points

	//Events
	void doEvent1(float delta);
	void doEvent2(float delta);
	void doEvent3(float delta);
	bool event2Helper();

	//Helpers
	void getScore(float r, glm::vec3 pos, int player);
	void loadScoreLocations();
	void initArena();
	void generateHeightField();
	void getWinner();
};

#endif