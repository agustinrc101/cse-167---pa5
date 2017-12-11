#include "Skybox.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Skybox::Skybox(GLint shad){
	std::vector<std::string> faces = 
			{	"./skybox/watercrater/rt.tga",
				"./skybox/watercrater/lf.tga",
				"./skybox/watercrater/up2.tga",
				"./skybox/watercrater/dn2.tga",
				"./skybox/watercrater/bk.tga",
				"./skybox/watercrater/ft.tga"
			};

	initVertices(3000.0f);
	loadCubeMap(faces);
	initCubeMap();
	//draw(shader);

	shader = shad;
}

Skybox::Skybox(GLint shad, float p) {
	std::vector<std::string> faces =
	{ "./skybox/watercrater/rt.tga",
		"./skybox/watercrater/lf.tga",
		"./skybox/watercrater/up2.tga",
		"./skybox/watercrater/dn2.tga",
		"./skybox/watercrater/bk.tga",
		"./skybox/watercrater/ft.tga"
	};

	initVertices(p);
	loadCubeMap(faces);
	initCubeMap();
	//draw(shader);

	shader = shad;
}


Skybox::~Skybox(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Skybox::loadCubeMap(std::vector<std::string> faces) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrcChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		helperPrint(i);
		unsigned char * data = stbi_load(faces[i].c_str(), &width, &height, &nrcChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			//stbi_image_free(data);
		}
		else {
			std::cout << "\tCubemap texture failed to load at path: " << faces[i].c_str() << "\n";
			//stbi_image_free(data);
		}
		//stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::initCubeMap() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &(vertices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Skybox::draw(GLint shader) {
	//glActiveTexture(GL_TEXTURE0);
	glDepthMask(GL_FALSE);
	glUseProgram(shader);

	glm::mat4 modelview = Window::V;
	uProjection = glGetUniformLocation(shader, "projection");
	uModelview = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//draw rest of the scene
}

void Skybox::initVertices(float p) {
	vertices = {
		//back
		{-p,  p, -p},
		{-p, -p, -p},
		{p, -p, -p},
		{p, -p, -p},
		{p,  p, -p},
		{-p,  p, -p},
		//left
		{-p, -p,  p},
		{-p, -p, -p},
		{-p,  p, -p},
		{-p,  p, -p},
		{-p,  p,  p},
		{-p, -p,  p},
		//right
		{p, -p, -p},
		{p, -p,  p},
		{p,  p,  p},
		{p,  p,  p},
		{p,  p, -p},
		{p, -p, -p},
		//front
		{-p, -p,  p},
		{-p,  p,  p},
		{p,  p,  p},
		{p,  p,  p},
		{p, -p,  p},
		{-p, -p,  p},
		//top
		{-p, p, -p},
		{p, p, -p},
		{p, p, p},
		{p, p, p},
		{-p, p, p},
		{-p, p, -p},
		//bottom
		{-p, -p, -p},
		{-p, -p,  p},
		{p, -p, -p},
		{p, -p, -p},
		{-p, -p,  p},
		{p, -p,  p}
	};
}

unsigned int Skybox::getTextureID() { return textureID; }
GLuint Skybox::getCubeMapVAO() { return VAO; }

void Skybox::helperPrint(int i) {
	switch (i) {
	case 0:
		std::cout << "\tLoading right texture...\n";
		break;
	case 1:
		std::cout << "\tLoading left texture...\n";
		break;
	case 2:
		std::cout << "\tLoading top texture...\n";
		break;
	case 3:
		std::cout << "\tLoading bottom texture...\n";
		break;
	case 4:
		std::cout << "\tLoading back texture...\n";
		break;
	case 5:
		std::cout << "\tLoading front texture...\n";
		break;
	default:
		std::cout << "\tERROR - invalid texture index\n";
		break;
	}
}