#include "Transform.h"
#include "Window.h"

Transform::Transform() {
	power = 1;
	M = glm::mat4(1.0f);
	parent = NULL;
	curAngle = 0.0f;
}

Transform::Transform(Node * p) {
	power = 1;
	M = glm::mat4(1.0f);
	parent = p;
	curAngle = 0.0f;
}

Transform::Transform(const Transform& t) {
	M = t.M;
	power = t.power;
	isLimb = t.isLimb;
	side = t.side;
	curAngle = t.curAngle;
	dir = t.dir;
	singleRotate = t.singleRotate;
	parent = t.parent;
	children = t.children;
}

Transform::~Transform() {
	for (std::list<Node *>::iterator it = children.begin(); it != children.end(); it++) {
		delete(*it);
	}
}

void Transform::draw(glm::mat4 C) {
	glm::mat4 new_M = C * M;

	for (std::list<Node *>::iterator it = children.begin(); it != children.end(); it++) {
		Node * temp = *it;
		if (temp == NULL)
			std::cout << "ERROR drawing transform\n";
		temp->draw(new_M);
	}
}

void Transform::update(){
	for (std::list<Node *>::iterator it = children.begin(); it != children.end(); it++) {
		Node * temp = *it;
		if (temp == NULL)
			std::cout << "ERROR updating transform\n";
		temp->update();
	}
}

void Transform::walk() {
	
}

bool Transform::addChild(Node * n) {
	if (n == NULL) { return false; }
	
	children.push_back(n);
	return true;
}

bool Transform::removeChild() {
	if (children.empty()) { return false; }

	children.pop_back();
	return true;
}

void Transform::scale(float x, float y, float z) {
	M = M * glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Transform::rotate(glm::vec4 rt) {
	M = glm::rotate(glm::mat4(1.0f), rt.x / 180.0f * glm::pi<float>(), glm::vec3(rt.y, rt.z, rt.w)) * M;
}

void Transform::translate(float x, float y, float z) {
	M = M * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Transform::translate(glm::vec3 tr) {
	M = M * glm::translate(glm::mat4(1.0f), tr);
}

void Transform::setPos(glm::vec3 pos) {
	glm::vec4 p(pos, 1.0f);
	M[3] = p;
}

void Transform::setCulling(bool b, int mode) {

}