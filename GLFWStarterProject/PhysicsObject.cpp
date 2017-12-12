#include "PhysicsObject.h"
#include "../Window.h"	//Can pass in V and P as alternative (depends on the shader)

PhysicsObject::PhysicsObject(const char * filepath, GLuint shader, GLuint bordShader, int t, int player){
	M = glm::mat4(1.0f);	//Initializes transformation matrix
	parse(filepath);		//Parses the model
	initModel();			//Loads model into a buffer
	shaderProgram = shader;
	borderShader = bordShader;
	type = t;
	this->player = player;
}

PhysicsObject::PhysicsObject(GLuint shader, GLuint bordShader, int t, std::vector<std::vector<float>> terrain) {
	type = t; 
	isPlayer = false; 
	initHeightMapModel(terrain); 
	shaderProgram = shader;
	borderShader = bordShader;
}

PhysicsObject::~PhysicsObject(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO2);
}

void PhysicsObject::draw(glm::mat4 C) {
	modelview = C * M;
	V = C;

	if (type == PhysicsEngine::TYPE_CYLINDER) {		//Draws thrown objects
		if(player == 0)
			render(glm::vec3(1.0, 1.0, 0.0));
		else if(player == 1)
			render(glm::vec3(1.0, 0.0, 0.0));
		else if(player == 2)
			render(glm::vec3(0.0, 0.0, 1.0));
		else if (player == 3)
			render(glm::vec3(0.0, 1.0, 0.0));
	}
	else if (type == PhysicsEngine::TYPE_HEIGHTFIELD)	//Draws HeightField (can be done elsewhere)
		render(glm::vec3(0.9, 0.4, 1.0));		
}

//Rendering and Parsing
void PhysicsObject::initModel() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO2);

	//passes vertices
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &(vertices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//passes indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//passes normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &(normals[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PhysicsObject::initHeightMapModel(std::vector<std::vector<float>> terrain) {
	float height = 2.0f;
	float size = 1.0f;

	for (int i = 0; i < terrain.size() - 1; i++) {
		for (int j = 0; j < terrain[i].size() - 1; j++) {
			float x = i*size - (terrain.size() / 2.0f) + 1.0f;
			float y = terrain[i][j] * height;
			float z = j*size - (terrain.size() / 2.0f) + 1.0f;

			vertices.push_back(glm::vec3(x, y, z));
		}
	}
	int max = 0;
	for (int i = 0; i < terrain.size() - 1; i++) {
		for (int j = 0; j < terrain[i].size() - 1; j++) {
			int start = i * terrain[i].size() + j;
			indices.push_back(start);
			indices.push_back(start + 1);
			indices.push_back(start + terrain[i].size());

			indices.push_back(start + 1);
			indices.push_back(start + 1 + terrain[i].size());
			indices.push_back(start + terrain[i].size());
		
			if (start + 1 + terrain[i].size() > max) max = start + 1 + terrain[i].size();
		}
	}

	for (int i = 0; i < indices.size(); i += 3) {
		glm::vec3 A = vertices[indices[i]];
		glm::vec3 B = vertices[indices[i+1]];
		glm::vec3 C = vertices[indices[i+2]];

		normals.push_back(glm::cross(B - A, C - A));
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO2);

	//passes vertices
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &(vertices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//passes indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//passes normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &(normals[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PhysicsObject::render() {
	glUseProgram(borderShader);
	glBindVertexArray(VAO);

	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uView = glGetUniformLocation(shaderProgram, "view");
	uModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);	//Perspective matrix
	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);				//View matrix
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);				//Model matrix
	
	uCol = glGetUniformLocation(borderShader, "color");
	uOffset = glGetUniformLocation(borderShader, "offset");
	// RENDER BLACK ENLARGED MESH
	glEnable(GL_CULL_FACE); // enable culling
	glCullFace(GL_CW); // enable culling of front faces
	glDepthMask(GL_TRUE); // enable writes to Z-buffer
	glUniform3f(uCol, 0.0f, 0.0f, 0.0f); // black colour
	glUniform1f(uOffset, 0.5f); // offset along normal
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL); // draw mesh

}

void PhysicsObject::render(glm::vec3 rgb) {
	render();
	glUseProgram(shaderProgram);

	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uView = glGetUniformLocation(shaderProgram, "view");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uCamPos = glGetUniformLocation(shaderProgram, "camPos");
	uCol = glGetUniformLocation(shaderProgram, "rgb");
	uType = glGetUniformLocation(shaderProgram, "type");
	uCent100 = glGetUniformLocation(shaderProgram, "center100");
	uCent50 = glGetUniformLocation(shaderProgram, "center50");
	uCent25 = glGetUniformLocation(shaderProgram, "center25");
	uRad100 = glGetUniformLocation(shaderProgram, "radius100");
	uRad50 = glGetUniformLocation(shaderProgram, "radius50");
	uRad25 = glGetUniformLocation(shaderProgram, "radius25");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);	//Perspective Matrix
	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);				//View Matrix
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);				//Model matrix
	glUniform3f(uCamPos, campos.x, campos.y, campos.z);
	glUniform3f(uCol, rgb.x, rgb.y, rgb.z);
	glUniform1i(uType, type);
	glUniform3f(uCent100, cent100.x, cent100.y, cent100.z);
	glUniform3f(uCent50, cent50.x, cent50.y, cent50.z);
	glUniform3f(uCent25, cent25.x, cent25.y, cent25.z);
	glUniform1f(uRad100, rad100);
	glUniform1f(uRad50, rad50);
	glUniform1f(uRad25, rad25);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void PhysicsObject::parse(const char *filepath) {
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
	FILE * file;		        // File pointer
	float x, y, z;		        // vertex coordinates
	float r, g, b;			    // vertex colors
	int i1, i2, i3, i4, i5, i6;	// Indeces for triangles
	int c1, c2;			        // characters read from file
	float maxX = 0;
	float maxY = 0;
	float maxZ = 0;
	float minX = 0;
	float minY = 0;
	float minZ = 0;


	//std::cout << "\tReading " << filepath << "\n";

	file = fopen(filepath, "rb");



	if (file == NULL) {
		std::cerr << "\nERROR - loading file " << filepath << std::endl;
		exit(-1);
	}

	c1 = fgetc(file);
	c2 = fgetc(file);

	while (c1 != EOF && c2 != EOF) {
		if (c1 == '#' || c2 == '#') {
			while (c1 != '\n' && c1 != EOF)
				c1 = fgetc(file);
		}
		else if (c1 == 'v' && c2 == ' ') {
			fscanf(file, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
			vertices.push_back(glm::vec3(x, y, z));
			if (x > maxX) { maxX = x; }
			else if (x < minX) { minX = x; }
			if (y > maxY) { maxY = y; }
			else if (y < minY) { minY = y; }
			if (z > maxZ) { maxZ = z; }
			else if (z < minZ) { minZ = z; }
		}
		else if (c1 == 'v' && c2 == 'n') {
			fscanf(file, "%f %f %f", &x, &y, &z);
			normals.push_back(glm::vec3(x, y, z));
		}
		else if (c1 == 'f' || c2 == ' ') {
			fscanf(file, "%d//%d %d//%d %d//%d", &i1, &i2, &i3, &i4, &i5, &i6);
			indices.push_back(i1 - 1);
			indices.push_back(i3 - 1);
			indices.push_back(i5 - 1);
		}

		if (c1 != EOF && c2 != EOF) {
			c1 = fgetc(file);
			c2 = fgetc(file);
		}
	}

	fclose(file);
	//std::cout << "\t" << filepath << ", vertices: " << vertices.size() << ", normals: " << normals.size() << ", faces: " << (indices.size() / 3)
	//	<< ", maxX: " << maxX << ", minX: " << minX << std::endl;

	hExtents.x = (glm::abs(maxX) + glm::abs(minX)) / 2;
	hExtents.y = (glm::abs(maxY) + glm::abs(minY)) / 2;
	hExtents.z = (glm::abs(maxZ) + glm::abs(minZ)) / 2;
}

void PhysicsObject::setScoreRadii(float r1, float r2, float r3) {
	rad100 = r1;
	rad50 = r2;
	rad25 = r3;
}

void PhysicsObject::setScoreCenters(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3) {
	cent100 = c1;
	cent50 = c2;
	cent25 = c3;
}