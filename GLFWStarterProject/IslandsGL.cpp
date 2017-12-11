#include "IslandsGL.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <iostream>

IslandsGL::~IslandsGL(){
	delete(score100);
	delete(score50);
	delete(score25);
	delete(physicsEngine);
}

void IslandsGL::addHeightField(GLuint shader, GLuint bordShader, std::vector<std::vector<float>> field, int size, glm::vec3 pos) {
	physicsEngine->addHeightField(shader, bordShader, field, size, pos);
}

void IslandsGL::draw(glm::mat4 C) {
	physicsEngine->draw(C);
}

void IslandsGL::update(float delta) {
	if (!paused) {
		if (event2 || event3 || gameIsOver)
			physicsEngine->update(delta);

		if (event1)
			doEvent1(delta);
		else if (event2)
			doEvent2(delta);
		else if (event3)
			doEvent3(delta);

		if (gameIsOver) {
			event1 = event2 = event3 = false;
			//cam->setPos(glm::vec3(0.0f, 30.0f, 0.0f));
		}
	}
}

glm::vec3 IslandsGL::getScoreCenter(int score) {
	if (score == 100)
		return score100->position;
	else if (score == 50)
		return score50->position;
	else if (score == 25)
		return score25->position;
	else
		return glm::vec3(0.0f);
}

float IslandsGL::getScoreRadius(int score) {
	if (score == 100)
		return score100->radius;
	else if (score == 50)
		return score50->radius;
	else if (score == 25)
		return score25->radius;
	else
		return 0;
}

void IslandsGL::loadScoreLocations() {
	std::cout << "\tGenerating ScorePoint Locations...\n";
	
	time_t t = clock();
	srand(t);

	score100->radius = 4.0f;
	score50->radius = 6.0f;
	score25->radius = 8.0f;

	std::cout << "\t0%\n";
	//Set score25's position
	bool isInside = false;
	while (!isInside) {
		float x = rand() % ((int)(2 * radius) + 1) - radius;
		float z = rand() % ((int)(2 * radius) + 1) - radius;
		score25->position = glm::vec3(x, 0, z);

		if (glm::distance(glm::vec3(center.x, 0, center.z), score25->position) < radius + score25->radius)
			isInside = true;
	}
	std::cout << "\t12%\n";

	//Set score50's position
	isInside = false;
	while (!isInside) {
		float x = rand() % ((int)(2 * radius) + 1) - radius;
		float z = rand() % ((int)(2 * radius) + 1) - radius;
		score50->position = glm::vec3(x, 0, z);

		if (glm::distance(center, score50->position) < radius + score50->radius){
			if (glm::distance(score50->position, score25->position) >= score50->radius + score25->radius)
				isInside = true;
		}
	}

	std::cout << "\t50%\n";

	//Set score100's position
	isInside = false;
	while (!isInside) {
		float x = rand() % ((int)(2 * radius) + 1) - radius;
		float z = rand() % ((int)(2 * radius) + 1) - radius;
		score100->position = glm::vec3(x, 0, z);

		if (glm::distance(center, score100->position) < radius + score100->radius) {
			if (glm::distance(score50->position, score100->position) >= score50->radius + score100->radius) {
				if(glm::distance(score25->position, score100->position) >= score25->radius + score100->radius)
					isInside = true;
			}
		}
	}
	std::cout << "\t100%\n";
}

void IslandsGL::initArena() {
	physicsEngine->setScoreRadii(score100->radius, score50->radius, score25->radius);
	physicsEngine->setScoreCenters(score100->position, score50->position, score25->position);
	
	event1 = true;
	canAddModels = true;
	initialPos = glm::vec3(0.0f, 20.0f, -105.0f);
}

void IslandsGL::generateHeightField() {
	std::cout << "\n\tGenerating Plane\n";
	physicsEngine->addPlane();
	
	std::cout << "\tGenerating heightfield\n";
	int siz = (int)radius * 2;
	std::vector<std::vector<float>>field;
	std::vector<float> temp;
	time_t t = clock();
	srand(t);
	for (int i = 0; i < siz; i++) {
		temp.clear();
		for (int j = 0; j < siz; j++) {
			temp.push_back((float)(rand() % 256) / 250.0f);
		}
		field.push_back(temp);
	}

	physicsEngine->addHeightField(cellShader, borderShader, field, siz, glm::vec3(0.0f, -10.0f, 0.0f));
}

void IslandsGL::mouseMovement(float x, float y) {
	int size = physicsEngine->getObjectsSize();
	physicsEngine->moveObject(glm::vec3(-x / 20.0f, y / 20.0f, 0.0f), size - 1);
	physicsEngine->moveObject(glm::vec3(-x / 20.0f, y / 20.0f, 0.0f), size - 2);
	physicsEngine->moveObject(glm::vec3(-x / 20.0f, y / 20.0f, 0.0f), size - 3);
}

void IslandsGL::doEvent1(float delta) {
	if (canAddModels) {
		canAddModels = false;
		strength = 0;
		physicsEngine->addObject(model, cellShader, borderShader, 1, curPlayer, initialPos + glm::vec3(0.0f, -5.0f, 15.0f));
		physicsEngine->addObject(model, cellShader, borderShader, 1, curPlayer, initialPos + glm::vec3(-5.0f, -5.0f, 15.0f));
		physicsEngine->addObject(model, cellShader, borderShader, 1, curPlayer, initialPos + glm::vec3(5.0f, -5.0f, 15.0f));

		xMoved = 0.0f;
		yMoved = 0.0f;
	}

	if (launched) {
		event1 = false;
		event2 = true;
		launched = false;

		int index = physicsEngine->getObjectsSize();
		glm::vec3 pos1 = physicsEngine->getPosition(index - 1);
		glm::vec3 pos2 = physicsEngine->getPosition(index - 2);
		glm::vec3 pos3 = physicsEngine->getPosition(index - 3);

		physicsEngine->launch(glm::vec3(pos1.x, pos1.y + 5.0f, pos1.z + 5.0f), glm::abs(yMoved), index - 1);
		physicsEngine->launch(glm::vec3(pos2.x, pos2.y + 5.0f, pos2.z + 5.0f), glm::abs(yMoved), index - 2);
		physicsEngine->launch(glm::vec3(pos3.x, pos3.y + 5.0f, pos3.z + 5.0f), glm::abs(yMoved), index - 3);
	}

	gameEllapsedTime = delta;
}

glm::vec3 IslandsGL::getNewCamPos() {
	glm::vec3 p = physicsEngine->getPosition(physicsEngine->getObjectsSize() - 3);
	return glm::vec3(p.x, p.y + 5.0f, p.z - 15.0f);
}

void IslandsGL::doEvent2(float delta) {
	bool track = event2Helper();

	if (delta - gameEllapsedTime < 0.8 || track) {}
	else{
		event2 = false;
		event3 = true;
	}
}

void IslandsGL::doEvent3(float delta) {
	event3 = false;
	event1 = true;
	canAddModels = true;
	curRound++;
	curPlayer++;
	if (curPlayer >= 4) curPlayer = 0;

	//CalculateScores
	p1Score = p2Score = p3Score = p4Score = 0;

	for (int i = 0; i < physicsEngine->getObjectsSize(); i++) {
		if (physicsEngine->getIsPlayer(i))
			getScore(physicsEngine->getRadius(i), physicsEngine->getPosition(i), physicsEngine->getPlayer(i));
	}
	std::cout << "======================" << std::endl;
	std::cout << "Yellow's Score: " << p1Score << std::endl;
	std::cout << "Red's Score:    " << p2Score << std::endl;
	std::cout << "Blue's Score:   " << p3Score << std::endl;
	std::cout << "Green's Score:  " << p4Score << std::endl;
	std::cout << "======================" << std::endl;

	if (curRound >(3 * 4)) gameIsOver = true; //MaxRounds is 3 (4 players)

	if (gameIsOver) {
		getWinner();
		event1 = false; 
		canAddModels = false;
	}
}

bool IslandsGL::event2Helper() {
	int size = physicsEngine->getObjectsSize();
	glm::vec3 vel1 = physicsEngine->getVelocity(size - 1);
	glm::vec3 vel2 = physicsEngine->getVelocity(size - 2);
	glm::vec3 vel3 = physicsEngine->getVelocity(size - 3);
	glm::vec3 pos1 = physicsEngine->getPosition(size - 1);
	glm::vec3 pos2 = physicsEngine->getPosition(size - 2);
	glm::vec3 pos3 = physicsEngine->getPosition(size - 3);

	bool track1 = false;
	bool track2 = false;
	bool track3 = false;

	if (glm::length(vel1) > 0.1f && pos1.y > -10.0f)
		track1 = true;
	if (glm::length(vel2) > 0.1f && pos2.y > -10.0f)
		track2 = true;
	if (glm::length(vel3) > 0.1f && pos3.y > -10.0f)
		track3 = true;

	return (track1 || track2 || track3);
}

void IslandsGL::getScore(float r, glm::vec3 pos, int player) {
	glm::vec3 tempPos = glm::vec3(pos.x, 0, pos.z);
	
	int score = 0;
	if (pos.y > -25.0f) {
		if (glm::distance(tempPos, score100->position) <= r + score100->radius)
			score = 100;
		else if (glm::distance(tempPos, score50->position) <= r + score50->radius)
			score = 50;
		else if (glm::distance(tempPos, score25->position) <= r + score25->radius)
			score = 25;
		else if (glm::distance(tempPos, center) < r + radius)
			score = 10;
		else
			score = 0;
	}

	
	//std::printf("X: %f, Z: %f\n", tempPos.x, tempPos.z);

	switch (player) {
	case 0:
		p1Score += score;
		break;
	case 1:
		p2Score += score;
		break;
	case 2:
		p3Score += score;
		break;
	case 3:
		p4Score += score;
		break;
	default:
		break;
	}
}

void IslandsGL::getWinner() {
	int max = glm::max(p1Score, p2Score);
	max = glm::max(max, p3Score);
	max = glm::max(max, p4Score);

	int counter = 0;
	if (p1Score == max)
		counter++;
	if (p2Score == max)
		counter++;
	if (p3Score == max)
		counter++;
	if (p4Score == max)
		counter++;

	if (counter == 1) {
		if (p1Score == max) {
			std::cout << "Yellow Wins!\n";
			return;
		}
		if (p2Score == max) {
			std::cout << "Red Wins!\n";
			return;
		}
		if (p3Score == max) {
			std::cout << "Blue Wins!\n";
			return;
		}
		if (p4Score == max) {
			std::cout << "Green Wins!\n";
			return;
		}
	}
	else {
		std::cout << "---------Tie!---------\n";
		while (counter > 0) {
			if (p1Score == max) {
				std::cout << "Yellow";
				counter--;
			}

			counter == 1 ? (std::cout << " and ") : (std::cout << ", ");
			if (p2Score == max) {
				std::cout << "Red";
				counter--;
			}

			counter == 1 ? (std::cout << " and ") : (std::cout << ", ");

			if (p3Score == max) {
				std::cout << "Blue";
				counter--;
			}

			std::cout << " and ";

			if (p4Score == max) {
				std::cout << "Green";
				counter--;
			}

			std::cout << "win!\n";
		}
	}
}