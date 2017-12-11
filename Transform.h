#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <list>

struct Node {
public:
	virtual void draw(glm::mat4 C) = 0;
	virtual void update() = 0;
	virtual void setCulling(bool b, int mode) = 0;
	virtual glm::mat4 getM() = 0;

protected:
	const char * name;
};

class Transform: public Node {
public:
	Node * parent;
	std::list<Node *> children;

	Transform();
	Transform(const Transform& t);
	Transform(Node * p);
	~Transform();

	void draw(glm::mat4 C);
	void update();
	bool addChild(Node * n);
	bool removeChild();
	void scale(float x, float y, float z);
	void rotate(glm::vec4 rt);
	void translate(float x, float y, float z);
	void translate(glm::vec3 tr);
	void setM(glm::mat4 m) { M = m; }
	void setPos(glm::vec3 pos);
	glm::mat4 getM() { return M; }
	bool getIsLimb() { return isLimb; }
	void walk();
	void setCulling(bool b, int mode);
	bool getIsCulling() { return isCulling; }
	Node * getParent() { return parent; }
	glm::vec3 getPos() { return M[3]; }


private:
	glm::mat4 M;
	int power;

	bool isLimb = false;
	int side;		//- or +
	float curAngle;
	int dir;		//dir if isLimb, radius if not
	bool singleRotate = true;
	bool isCulling = true;
};

#endif