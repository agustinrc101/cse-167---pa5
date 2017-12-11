#ifndef _PHYSICSENGINE_H_
#define _PHYSICSENGINE_H_

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

#include <vector>
#include <stdio.h>
#include <iostream>

#include "btBulletDynamicsCommon.h"
#include "BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"
#include "PhysicsObject.h"

class PhysicsEngine{
public:
	enum {
		TYPE_CYLINDER, TYPE_PLANE, TYPE_HEIGHTFIELD
	};

	PhysicsEngine();
	~PhysicsEngine();

	//Frame functions
	void update(float delta);
	void draw(glm::mat4 C);

	//.obj filename, model's shader, object mass (0 for terrain, higher for mesh), pos is center
	void addObject(const char * filename, GLuint shader, GLuint bordShader, btScalar mass, int player, glm::vec3 pos=glm::vec3(0.0f, 0.0f, 0.0f));
	void addPlane(glm::vec3 pos = glm::vec3(0.0f, -50.0f, 0.0f));
	void addHeightField(GLuint shader, GLuint bordShader, std::vector<std::vector<float>> field, int size, glm::vec3 pos=glm::vec3(0.0f, 0.0f, 0.0f));

	//Getters
	int getObjectsSize() { return objects.size(); }
	bool getIsPlayer(int index) { return objects[index]->getIsPlayer(); }
	float getRadius(int index) { return glm::max(objects[index]->getHalfExtents().x, objects[index]->getHalfExtents().z); }
	glm::vec3 getPosition(int index) { return objects[index]->getPosition(); }
	glm::mat4 getM(int index) { return objects[index]->getM(); }
	glm::vec3 getVelocity(int index) { btVector3 v = bodies[index]->getLinearVelocity(); return glm::vec3(v.getX(), v.getY(), v.getZ()); }
	int getPlayer(int index) { return objects[index]->getPlayer(); }

	//Setters
	void setGravity(float y) { dynamicsWorld->setGravity(btVector3(0.0, y, 0.0));; }
	void setScoreRadii(float r1, float r2, float r3) { for (int i = 0; i < objects.size(); i++) objects[i]->setScoreRadii(r1, r2, r3); }
	void setScoreCenters(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3) { for (int i = 0; i < objects.size(); i++) objects[i]->setScoreCenters(c1, c2, c3); }

	//Other functions
	void shoot(const char * filename, GLuint shader, GLuint bordShader, btScalar mass, glm::vec3 pos, glm::vec3 dir);
	void launch(glm::vec3 dir, int strength, int index);
	void updateCamPos(glm::vec3);

	//Transformation Functions (These DONT change the collision data (Use only for testing))
	void scaleObject(glm::vec3 s, int i) { objects[i]->scaleModel(s); }
	void moveObject(glm::vec3 t, int i);

private:
	//Bullet Physics
	btDefaultCollisionConfiguration* collisionConfiguration;	//
	btCollisionDispatcher* dispatcher;							//
	btBroadphaseInterface* overlappingPairCache;				//
	btSequentialImpulseConstraintSolver* solver;				//
	btAlignedObjectArray<btCollisionShape *> collisionShapes;	//Array of collision shapes
	btDiscreteDynamicsWorld * dynamicsWorld;					//


	std::vector<btRigidBody *> bodies;		//List of rigidBodies
	std::vector<PhysicsObject *> objects;	//List of objects to draw
	std::vector<PhysicsObject *> models;	//List of non-terrain models

	//Helper functions
	void printOutVec3(glm::vec3 v) { std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n"; }
};

#endif

