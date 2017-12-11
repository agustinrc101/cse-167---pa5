#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(){
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -5.0, 0));
}

PhysicsEngine::~PhysicsEngine(){
	/*
	//Removes and deletes rigidbodies
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i++) {
		btCollisionObject * obj = dynamicsWorld->getCollisionObjectArray()[i];//CRASH HERE
		btRigidBody * body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
	*/

	for (int i = 0; i < bodies.size(); i++) {
		btRigidBody * body = bodies[i];
		if (body && body->getMotionState())
			delete body->getMotionState();
		//remove collision object
		delete body;
	}

	for (int i = 0; i < collisionShapes.size(); i++) {
		btCollisionShape * shape = collisionShapes[i];
		collisionShapes[i] = 0;
		delete shape;
	}

	//Deletes Models
	for (int i = 0; i < objects.size(); i++) {
		delete(objects[i]);
	}

	//Deletes all other variables
	delete dispatcher;
	delete collisionConfiguration;
	delete solver;
	delete overlappingPairCache;
	//delete dynamicsWorld;//CRASHES
	collisionShapes.clear();
}

/* CYLINDER */
void PhysicsEngine::addObject(const char * filepath, GLuint shader, GLuint bordShader, btScalar mass, int player, glm::vec3 pos) {
	//Initializes object in the rendering pipeline
	PhysicsObject * obj = new PhysicsObject(filepath, shader, bordShader, TYPE_CYLINDER, player);
	obj->setPosition(pos);
	obj->setIsPlayer(mass);
	objects.push_back(obj);

	//Initializes object in the graphics engine pipeline
	btMotionState * ms;
	btCollisionShape * cs;
	btVector3 inertia(0, 0, 0);

	//Cylinder Shape
	glm::vec3 h = objects[objects.size() - 1]->getHalfExtents();
	btVector3 hExtents(h.x, h.y, h.z);

	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(pos.x, pos.y, pos.z));	//Sets origin position
	cs = new btCylinderShape(hExtents);				//mass > 0 means obj is affected by forces
	ms = new btDefaultMotionState(t);
	cs->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo info(mass, ms, cs, inertia);	//Body definition

	btRigidBody * rb = new btRigidBody(info);
	dynamicsWorld->addRigidBody(rb);
	bodies.push_back(rb);
}

/* PLANE */
void PhysicsEngine::addPlane(glm::vec3 pos) {
	objects.push_back(new PhysicsObject(TYPE_PLANE));

	btMotionState * ms;
	btCollisionShape * cs;
	btVector3 inertia(0, 0, 0);
	btTransform t;

	t.setIdentity();
	t.setOrigin(btVector3(pos.x, pos.y, pos.z));	//Sets origin position
	cs = new btStaticPlaneShape(btVector3(0, 1, 0), pos.y);
	ms = new btDefaultMotionState(t);

	btRigidBody::btRigidBodyConstructionInfo info(0, ms, cs, inertia);	//Body definition

	btRigidBody * rb = new btRigidBody(info);
	dynamicsWorld->addRigidBody(rb);
	bodies.push_back(rb);

	//Sets friction
	bodies[bodies.size() - 1]->setFriction(0.95);
}

/* HEIGHT FIELD */		//TODO update to make it easier to draw arena
void PhysicsEngine::addHeightField(GLuint shader, GLuint bordShader, std::vector<std::vector<float>> field, int size, glm::vec3 pos) {
	PhysicsObject * hf = new PhysicsObject(shader, bordShader, TYPE_HEIGHTFIELD, field);
	hf->setPosition(glm::vec3(pos.x, pos.y - 2.0f, pos.z));
	objects.push_back(hf);


	//Initializes object in the graphics engine pipeline
	btMotionState * ms;
	btCollisionShape * cs;
	btVector3 inertia(0, 0, 0);
	btTransform t;

	if (size == 100) {
		btScalar heightField[100][100];
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				heightField[i][j] = field[i][j];
			}
		}

		t.setIdentity();
		t.setOrigin(btVector3(pos.x, pos.y, pos.z));	//Sets origin position
		cs = new btHeightfieldTerrainShape(100, 100, heightField, 1, 0, 2, 1, PHY_FLOAT, true);
	}
	else {
		btScalar heightField[200][200];
		for (int i = 0; i < 200; i++) {
			for (int j = 0; j < 200; j++) {
				heightField[i][j] = field[i][j];
			}
		}

		t.setIdentity();
		t.setOrigin(btVector3(pos.x, pos.y, pos.z));	//Sets origin position
		cs = new btHeightfieldTerrainShape(200, 200, heightField, 1, -10, 30, 1, PHY_FLOAT, true);	
	}

	ms = new btDefaultMotionState(t);

	btRigidBody::btRigidBodyConstructionInfo info(0, ms, cs, inertia);	//Body definition

	btRigidBody * rb = new btRigidBody(info);
	dynamicsWorld->addRigidBody(rb);
	bodies.push_back(rb);

	//Sets friction
	bodies[bodies.size() - 1]->setFriction(0.95);
}

//TODO do something about delay
void PhysicsEngine::update(float delta) {
	dynamicsWorld->stepSimulation(1.0 / 60.0);
}

void PhysicsEngine::draw(glm::mat4 C) {
	for (int i = 0; i < bodies.size(); i++) {
		if (objects[i]->getIsPlayer()) {
			glm::mat4 m(1.0f);
			//bodies[i]->getWorldTransform().getOpenGLMatrix(s);
			btTransform currentTransform = btTransform();
			bodies[i]->getMotionState()->getWorldTransform(currentTransform);

			GLfloat * ModelMatrix = new GLfloat[16];
			currentTransform.getOpenGLMatrix(ModelMatrix);
			m[0][0] = ModelMatrix[0];m[1][0] = ModelMatrix[4];m[2][0] = ModelMatrix[8]; m[3][0] = ModelMatrix[12];
			m[0][1] = ModelMatrix[1];m[1][1] = ModelMatrix[5];m[2][1] = ModelMatrix[9]; m[3][1] = ModelMatrix[13];
			m[0][2] = ModelMatrix[2];m[1][2] = ModelMatrix[6];m[2][2] = ModelMatrix[10];m[3][2] = ModelMatrix[14];
			m[0][3] = ModelMatrix[3];m[1][3] = ModelMatrix[7];m[2][3] = ModelMatrix[11];m[3][3] = ModelMatrix[15];

			objects[i]->setM(m);
		}
	}
	
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->draw(C);
	}
}

void PhysicsEngine::shoot(const char * filename, GLuint shader, GLuint bordShader, btScalar mass, glm::vec3 pos, glm::vec3 dir) {
	int index = objects.size();
	addObject(filename, shader, bordShader, mass, -1, pos);

	glm::vec3 l = dir - pos;
	l = glm::normalize(l);

	btVector3 launch = btVector3(l.x * 10.0, l.y * 10.0, l.z * 10.0);
	bodies[index]->setLinearVelocity(launch);
}

void PhysicsEngine::launch(glm::vec3 dir, int strength, int index) {
	PhysicsObject * temp = objects[index];
	if (!temp->getIsPlayer()) return;

	glm::vec3 l = dir - temp->getPosition();
	l = glm::normalize(l);
	btVector3 launch = btVector3(l.x, l.y * 20.0f, l.z * 1.0 * 10.0f);
	bodies[index]->setLinearVelocity(launch);
}

//Helpers
void PhysicsEngine::updateCamPos(glm::vec3 pos) {
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->updateCamPos(pos);
	}
}

void PhysicsEngine::moveObject(glm::vec3 t, int index) {
	//btTransform currentTransform = btTransform();
	//bodies[index]->getMotionState()->getWorldTransform(currentTransform);

	//btVector3 s = bodies[index]->getWorldTransform().getOrigin();
	//btVector3 e = btVector3(t.x, t.y, t.z);
	
	//bodies[index]->getWorldTransform().setOrigin(s + e);
	
	}