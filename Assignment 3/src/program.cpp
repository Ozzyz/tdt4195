// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "sphere.hpp"
#include "sceneGraph.hpp"

int indicesCount;
GLuint setupVAO(float*, int, int*, int, float*);
SceneNode* setupNodes();
glm::mat4 getView();
void check_pressed_keys();
void modelScene(SceneNode*);
void renderScene(SceneNode*);

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 70.0);  //xyz
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);  //vector indication direction camera points, used for movement relative to camera
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0); //global up, used to recalculate cameraRight
glm::vec3 cameraRight = glm::cross(up, cameraDirection);  //right vector relative to camera, for relative movement
glm::vec2 orientation(0.0, 0.0);  //store the camera rotation on the x and y axis

bool keys_pressed[1024];
int counter;


void runProgram(GLFWwindow* window)
{
    // Set GLFW callback mechanism(s)
    glfwSetKeyCallback(window, keyboardCallback);

    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.3f, 0.3f, 0.4f, 1.0f);

	float color[] = {
		1.0, 0.0, 1.0
	};

	SceneNode* sun = setupNodes();

	//GLuint VAO;

	//VAO = createCircleVAO(30, 30, color);

	// Setup shader
	Gloom::Shader shader;
	shader.makeBasicShader("../gloom/shaders/mvp.vert", "../gloom/shaders/colors.frag");
	// Boolean for more simple activation or deactivaton of shader when testing
	bool shaderON = true;
	
    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw your scene here
		if (shaderON){
			shader.activate();
			
			check_pressed_keys();

			cameraRight = glm::cross(cameraDirection, up); // update global cameraRight vector for strafing

			glm::mat4 view = getView();
			
			glm::mat4 projection = glm::perspective(glm::radians(60.0), 1.0, 1.0, 200.0);

			glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(projection));
		}

		modelScene(sun);
		renderScene(sun);

		if (shaderON){
		shader.deactivate();
		}

        // Handle other events
        glfwPollEvents();

        // Flip buffers
        glfwSwapBuffers(window);
    }

	if (shaderON){
	shader.destroy();
	}
}


GLuint setupVAO(float* vertices, int verticesSize, unsigned int* indices, int indicesSize, float* colors){
	// Generates a new Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Creates a vertexbuffer
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), colors, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Creates a indicebuffer
	GLuint indicebuffer;
	glGenBuffers(1, &indicebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicebuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// sets the global indicesSize for use when drawing
	indicesCount = indicesSize;

	return VertexArrayID;
}


SceneNode* setupNodes() {
	SceneNode* sun = createSceneNode();
	sun->rotationSpeedRadians = toRadians(5);
	sun->rotationDirection = glm::vec3(1.0, 1.0, 0.0);
	sun->scaleFactor = 2.0;
	float color[] = { 1.0, 1.0, 0.0 };
	sun->vertexArrayObjectID = createCircleVAO(30, 30, color);

	for (int i = 1; i < 6; i++) {
		SceneNode* planet = createSceneNode();
		addChild(sun, planet);
		planet->rotationSpeedRadians = toRadians(1 + 5 * random());
		if (i == 1 || i == 2 || i == 4) {
			planet->rotationDirection = glm::vec3(0, 1, 0);
		}

		else {
			planet->rotationDirection = glm::vec3(0, -1, 0);
		}
		
		planet->scaleFactor = 0.1 + 0.5 * random();
		planet->x = 5*i;
		color[0] = random(); color[1] = random(); color[2] = random();
		planet->vertexArrayObjectID = createCircleVAO(30, 30, color);

		if (i == 1 || i == 3) {
			SceneNode* moon = createSceneNode();
			addChild(planet, moon);
			moon->rotationSpeedRadians = toRadians(1 + 5 * random());
			moon->rotationDirection = glm::vec3(0, 1, 0);
			moon->scaleFactor = 0.1 + 0.5*random();
			moon->x = 3;
			color[0] = random(); color[1] = random(); color[2] = random();
			moon->vertexArrayObjectID = createCircleVAO(30, 30, color);
		}
	}
	return sun;
}


void modelScene(SceneNode* sun) {
	counter += 1;

	float time = getTimeDeltaSeconds();
	float scaleFactor = sun->scaleFactor;
	float rotateSpeed = sun->rotationSpeedRadians;
	glm::vec3 rotateDirection = sun->rotationDirection;
	glm::mat4 scale = glm::scale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	glm::mat4 rotate = glm::rotate(rotateSpeed*time, rotateDirection);

	glm::mat4 sunMatrix = rotate*scale;

	sun->currentTransformationMatrix = sunMatrix;

	//printNode(sun);

	for (SceneNode* planet : sun->children) {
		//printNode(planet);

		glm::mat4 translate = glm::translate(glm::vec3(planet->x, planet->y, planet->z));
		scaleFactor = planet->scaleFactor;
		rotateSpeed = planet->rotationSpeedRadians;
		rotateDirection = planet->rotationDirection;
		scale = glm::scale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		rotate = glm::rotate(rotateSpeed*counter / 100, rotateDirection);

		glm::mat4 planetMatrix = sunMatrix*rotate*translate*scale;
		planet->currentTransformationMatrix = planetMatrix;

		for (SceneNode* moon : planet->children) {
			translate = glm::translate(glm::vec3(moon->x, moon->y, moon->z));
			scaleFactor = moon->scaleFactor;
			rotateSpeed = moon->rotationSpeedRadians;
			rotateDirection = moon->rotationDirection;
			scale = glm::scale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
			rotate = glm::rotate(rotateSpeed*counter / 100, rotateDirection);

			glm::mat4 moonMatrix = planetMatrix*rotate*translate*scale;
			moon->currentTransformationMatrix = moonMatrix;
		}
	}
}


void renderScene(SceneNode* sun) {
	GLuint VAO = sun->vertexArrayObjectID;
	//glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(sun->currentTransformationMatrix));
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)0);

	for (SceneNode* planet : sun->children) {
		VAO = planet->vertexArrayObjectID;
		glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(planet->currentTransformationMatrix));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)0);

		for (SceneNode* moon : planet->children) {
			VAO = moon->vertexArrayObjectID;
			glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(moon->currentTransformationMatrix));
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)0);
		}
	}
}


glm::mat4 getView() {
	// Translate all objects in the worldspace relative to camera
	glm::mat4 translate = glm::translate(-cameraPos);
	// Scale objects, in this case by a static number 
	float scaleFactor = 2;
	glm::mat4 scale = glm::scale(glm::vec3(scaleFactor, scaleFactor, 1.0));

	// Rotate world
	glm::mat4 Xrotation = glm::rotate(orientation.x, cameraRight);
	glm::mat4 Yrotation = glm::rotate(orientation.y, up);

	return Yrotation*Xrotation*translate*scale;
}


void keyboardCallback(GLFWwindow* window, int key, int scancode,
	int action, int mods)
{
	// Use escape key for terminating the GLFW window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	keys_pressed[key] = (action != GLFW_RELEASE);
}


void check_pressed_keys() {
	// Check what keys_pressed are activated, and react accordingly
	// Note: This means that multiple keys_pressed kan be activated at once.

	// Sets how much the camera moves during each frame if key pressed
	float incrementValue = 0.005;

	// WASD will control forward/back/left/right
	if (keys_pressed[GLFW_KEY_W]) {
		cameraPos += incrementValue*cameraDirection;
	}
	if (keys_pressed[GLFW_KEY_A]) {
		cameraPos -= incrementValue*cameraRight;
	}
	if (keys_pressed[GLFW_KEY_S]) {
		cameraPos -= incrementValue*cameraDirection;
	}
	if (keys_pressed[GLFW_KEY_D]) {
		cameraPos += incrementValue*cameraRight;
	}
	// Arrow keys_pressed control rotation (L&R -> x-axis, U/D -> y-axis)
	if (keys_pressed[GLFW_KEY_LEFT]) {
		cameraDirection = glm::mat3(glm::rotate(incrementValue, up)) * cameraDirection;
		orientation.y -= incrementValue;
	}
	if (keys_pressed[GLFW_KEY_RIGHT]) {
		cameraDirection = glm::mat3(glm::rotate(-incrementValue, up)) * cameraDirection;
		orientation.y += incrementValue;
	}
	if (keys_pressed[GLFW_KEY_UP]) {
		cameraDirection = glm::mat3(glm::rotate(incrementValue, cameraRight)) * cameraDirection;
		orientation.x -= incrementValue;
	}
	if (keys_pressed[GLFW_KEY_DOWN]) {
		cameraDirection = glm::mat3(glm::rotate(-incrementValue, cameraRight)) * cameraDirection;
		orientation.x += incrementValue;
	}
	// Key presses for up and down movement, space for up and control for down
	if (keys_pressed[GLFW_KEY_SPACE]) {
		cameraPos.y += incrementValue;
	}
	if (keys_pressed[GLFW_KEY_LEFT_CONTROL]) {
		cameraPos.y -= incrementValue;
	}
}