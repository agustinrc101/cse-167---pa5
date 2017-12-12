#include "window.h"

const char* window_title = "PhysicsTesting";
GLint skyboxShader;
GLint toonShader;
GLint borderShader;

// On some systems you need to change this to the absolute path
#define SKYBOX_VERTEX_SHADER_PATH "../skyboxshader.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../skyboxshader.frag"
#define TOON_VERTEX_SHADER_PATH "../toonShader.vert"
#define TOON_FRAGMENT_SHADER_PATH "../toonShader.frag"
#define BORDER_VERTEX_SHADER_PATH "../borders.vert"
#define BORDER_FRAGMENT_SHADER_PATH "../borders.frag"

int Window::width;
int Window::height;
double xPos, yPos;

//Camera matrices and object pointers
glm::mat4 Window::P;
glm::mat4 Window::V;
Camera * cam;
Skybox * skybox;
IslandsGL * game;

//value trackers
float fixedEllapsedTime = 0.0f;
clock_t lastTime;

//Cursor callback vars
glm::vec3 old_cursor_pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 new_cursor_pos = glm::vec3(0.0f, 0.0f, 0.0f);

//Boolean toggles
bool lmbHold = false;
bool updateWorld = true;


void Window::initialize_objects() {
	// Load the shader program. Make sure you have the correct filepath up top
	skyboxShader = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);

	std::cout << "\n****LOADING SKYBOX****\n";
	skybox = new Skybox(skyboxShader);


	/*****COPY THIS ** change camera to the cool camera **************************/
	toonShader = LoadShaders(TOON_VERTEX_SHADER_PATH, TOON_FRAGMENT_SHADER_PATH);
	borderShader = LoadShaders(BORDER_VERTEX_SHADER_PATH, BORDER_FRAGMENT_SHADER_PATH);

	std::cout << "\n****LOADING GAME****\n";
	game = new IslandsGL(50.0f, glm::vec3(0.0f), toonShader, borderShader); //Must match the height field size
	std::cout << "\tDone...\n";
	//If you want to change the platform's height:
		//-In IslandsGL.cpp -> generateHeightFields(), change the y-pos
			//of the glm::vec3 in the last line of the method. 
		//-Offset the initial position of the balls by y in initArena()
		//-Offset the camera's initial position below and in idle callback (event1)


	//Add your height field here (might be rendered differently, however)
	//addHeightField();	// GLuint shader, GLuint bordShader, std::vector<std::vector<float>> field, int size, glm::vec3 pos=glm::vec3(0.0f, 0.0f, 0.0f)


	cam->Position = glm::vec3(0.0f, 5.0f, -105.0f);
	cam->Front = glm::vec3(0.0, 0.0, 0.0);
	V = cam->GetViewMatrix();								
	/**************************************************************************/
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up(){
	delete(skybox);
	delete(game);

	glDeleteProgram(skyboxShader);
	glDeleteProgram(toonShader);
	glDeleteProgram(borderShader);
}

GLFWwindow* Window::create_window(int width, int height){
	// Initialize GLFW
	if (!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window){
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);

	//Create the Camera
	std::cout << "\n****LOADING CAMERA****\n";
	cam = new Camera();
	V = cam->GetViewMatrix();

	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height){
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0 && cam != NULL){
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 10000.0f);
		V = cam->GetViewMatrix();
	}
}

void Window::idle_callback(){
	/********COPY THIS *****add missing variables to the top of this file**********************/
	if (!game->getIsPaused()) {
		if (lastTime == NULL) lastTime = clock();
		clock_t time = clock();
		float delta = float(time - lastTime) * 0.0001f;
		fixedEllapsedTime += delta;

		cam->Front = glm::vec3(0.0f, -0.5f, 1.0f);
		

		game->update(delta);
		if (game->getIsEvent1() && game->getNewCamPos().y > -10.0f)
			cam->Position = game->getNewCamPos();//glm::vec3(0.0f, 20.0f, -105.0f);	//Sets camera's initial position
		if (game->getIsEvent2() && cam->Position.z < -40.0f && game->getNewCamPos().y > -10.0f) //Camera follows the thrown objects
			cam->Position = game->getNewCamPos();
	}
	else {
		//What do update when the game is paused (toggle camera movement?)
	}

	V = cam->GetViewMatrix();
	/*****************************************************/
}

void Window::display_callback(GLFWwindow* window){
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the scene
	skybox->draw(skyboxShader);

	/***COPY THIS***********************************/
	game->draw(V);
	/**********************************************/


	// Swap buffers
	glfwSwapBuffers(window);
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	// Check for a key press
	if (action == GLFW_PRESS){
		// Check if escape was pressed - exits and closes the program
		if (key == GLFW_KEY_ESCAPE){
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

	/**************COPY THIS*******************************************************/
		//Checks if p is pressed - toggles update pause
		else if (key == GLFW_KEY_P) {
			game->togglePause();
		}
	/*******************************************************************************/
		else if (key == GLFW_KEY_Z) {
			
		}

		//Check if R was pressed - resets camera position
		else if (key == GLFW_KEY_R) {

		}

		//Check if space was pressed - shoots a ball to look at pos
		else if (key == GLFW_KEY_SPACE) {
	
		}
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//Can ignore this
	if (game->getGameIsOver() || game->getIsPaused()) {
		cam->ProcessMouseScroll(yoffset);
	}
}


/*********************COPY THIS********************************/
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			glfwGetCursorPos(window, (double*)&old_cursor_pos.x, (double*)&old_cursor_pos.y);
			glfwGetCursorPos(window, &xPos, &yPos);
			lmbHold = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (lmbHold && game->getIsEvent1() && !game->getIsPaused())
				game->setLaunched(true);

			lmbHold = false; 
		} 
	}
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	new_cursor_pos.x = (float)xpos;
	new_cursor_pos.y = (float)ypos;

	if (checkCursorPosition(new_cursor_pos, old_cursor_pos) && lmbHold) {
		leftCursorActions();
	}

	old_cursor_pos = new_cursor_pos;
}

bool Window::checkCursorPosition(glm::vec2 n, glm::vec2 o) {
	if (old_cursor_pos.x >= width && old_cursor_pos.x <= 0)
		return false;
	if (old_cursor_pos.y >= height && old_cursor_pos.y <= 0)
		return false;
	if (old_cursor_pos.x >= width && old_cursor_pos.x <= 0)
		return false;
	if (old_cursor_pos.y >= height && old_cursor_pos.y <= 0)
		return false;

	return true;
}

void Window::leftCursorActions() {
	float x = (float)(old_cursor_pos.x - new_cursor_pos.x);
	float y = (float)(old_cursor_pos.y - new_cursor_pos.y);
	game->mouseMovement(x, y);
}
/****************************************************************/

Camera * Window::getCam() {return cam; }
Skybox * Window::getSkybox() { return skybox; }
GLint Window::getSkyboxShaderProgram() { return skyboxShader; }