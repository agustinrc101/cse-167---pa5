#ifndef _WINDOW_H_
#define _WINDOW_H_

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

#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "Camera.h"
#include "Skybox.h"
#include "shader.h"
#include "IslandsGL.h"

#define SPHERERADIUS 32

class Window{
public:
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view

	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);

	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static GLint getSkyboxShaderProgram();
	
	static Camera * getCam();
	static Skybox * getSkybox();
	
private:
	static bool checkCursorPosition(glm::vec2 n, glm::vec2 o);
	static void leftCursorActions();
};

#endif
