// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
/*************************************
*********FORWARD DECLARATIONS*********
**************************************/
GLuint setupVAO(float*, int, int*, int);
GLuint task1b();
GLuint task1d();
void incrementCameraLookingAtX(float);
void incrementCameraLookingAtY(float);
void incrementCameraY(float);
void incrementCameraX(float);
void check_pressed_keys();
glm::mat4x4 lookAt(glm::vec3, glm::vec3);

// Array to keep track over what keys are pressed
bool keys_pressed[1024];
int indicesSize;
glm::vec3 cameraPos = glm::vec3(0, 0, 0);
glm::vec3 cameraLookingAt = glm::vec3(1, 1, 1);
// Represents pitch and yaw (x and y rotation)
glm::vec2 orientation(0, 0.0);


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
	GLuint VAO;
	VAO = task1b();

	// Setup shader
	Gloom::Shader shader;
  // TODO: Make these relative paths
	shader.makeBasicShader("/home/shomec/a/aasmunhb/Documents/Visuell/gloom/gloom/shaders/rgba4x4.vert", "/home/shomec/a/aasmunhb/Documents/Visuell/gloom/gloom/shaders/rgba.frag");
	// Boolean for more simple activation or deactivaton of shader when testing
	bool shader_is_activated = true;

  // 4x4 matrix to pass to shader
  //glm::mat4x4 m(1.0);

  // Rendering Loop
  while (!glfwWindowShouldClose(window))
  {

  	// Clear colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get transformationmatrix based of camera data
    glm::mat4x4 transformationMatrix = lookAt(cameraLookingAt, cameraPos);

  	if (shader_is_activated){shader.activate();}
    // Generate data pointer to values
    glUniformMatrix4fv(2, 1,GL_FALSE, glm::value_ptr(transformationMatrix));
    glBindVertexArray(VAO);
  	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, (void*)0);

  	if (shader_is_activated){shader.deactivate();}
    // Handle other events
    glfwPollEvents();
    check_pressed_keys();
    // Flip buffers
    glfwSwapBuffers(window);
  }
	if (shader_is_activated){shader.destroy();}
}

void keyboardCallback(GLFWwindow* window, int key, int scancode,
                      int action, int mods){
    // Use escape key for terminating the GLFW window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // Set key to false if released, else true
    keys_pressed[key] = (action != GLFW_RELEASE);
}

void check_pressed_keys(){
  // Check what keys_pressed are activated, and react accordingly
  // Note: This means that multiple keys_pressed kan be activated at once.

  // Sets how much the camera moves during each frame if key pressed
  // If negative, it will behave as a camera (inverted controls)
  // If positive, it will behave as if you are steering the triangle
  float incrementValue = -0.01;

  // WASD will control up/down/left/right (y and x axis)
  if(keys_pressed[GLFW_KEY_W]){
    cameraPos.y += incrementValue;
  }
  if(keys_pressed[GLFW_KEY_A]){
    cameraPos.x += incrementValue;
  }
  if(keys_pressed[GLFW_KEY_S]){
    cameraPos.y -= incrementValue;
  }
  if(keys_pressed[GLFW_KEY_D]){
    cameraPos.x -= incrementValue;
  }
  // Arrow keys_pressed control rotation (L&R -> x-axis, U/D -> y-axis)
  if(keys_pressed[GLFW_KEY_LEFT]){
    orientation.y += incrementValue;
    cameraLookingAt.x += incrementValue;
  }
  if(keys_pressed[GLFW_KEY_RIGHT]){
    orientation.y -= incrementValue;
    cameraLookingAt.x -= incrementValue;
  }
  if(keys_pressed[GLFW_KEY_UP]){
    orientation.x -= incrementValue;
    cameraLookingAt.y += incrementValue;
  }
  if(keys_pressed[GLFW_KEY_DOWN]){
    cameraLookingAt.y -= incrementValue;
    orientation.x += incrementValue;
  }
}



glm::mat4x4 lookAt(glm::vec3 cameraLookingAt, glm::vec3 cameraPos){
  glm::vec3 cameraDirection = cameraPos-cameraLookingAt;
  glm::vec3 up = glm::vec3(0,1,0);
  // Right relative to the camera
  glm::vec3 cameraRight = glm::cross(up, cameraDirection);
  // Up relative to the camera
  glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
  // Translate all objects in the worldspace relative to camera
  glm::mat4 viewMatrix = glm::translate(-cameraPos);
  // Rotate camera so that it is pointing down the -z axis
  float xrot = orientation[0];
  float yrot = orientation[1];
  glm::mat4 XrotationMatrix = glm::rotate(xrot, glm::vec3(1.0, 0.0, 0.0));
  glm::mat4 YrotationMatrix = glm::rotate(yrot, glm::vec3(0.0, 1.0, 0.0));

  return YrotationMatrix*XrotationMatrix*viewMatrix;
}

GLuint setupVAO(float* vertices, int verticesSize, int* indices, int indicesSize, float* RGBAvalues) {
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

	// Creates a indicebuffer
	GLuint indicebuffer;
	glGenBuffers(1, &indicebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicebuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(int), indices, GL_STATIC_DRAW);

  // Create a buffer for RGBAvalues
  GLuint RGBAbuffer;
  // Id to pass to vertex shader
  int RGBAbuffer_index = 1;
  glGenBuffers(1, &RGBAbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, RGBAbuffer);
  // Since we have an rgba array for each vertex, we pass in verticesSize here
  glBufferData(GL_ARRAY_BUFFER, verticesSize* sizeof(float), RGBAvalues, GL_STATIC_DRAW);
  // Set up pointer to our RGBA buffer-> VAO
  glVertexAttribPointer(RGBAbuffer_index, // index to pass to vertex shader
                        4, // num values per entry (we have R,G,B and A)
                        GL_FLOAT, // Datatype of values in rgba array
                        GL_FALSE, // normalized?
                        0,        // stride (bytes between each entry in buffer)
                        0        // bytes until first value in buffer
   );
   glEnableVertexAttribArray(RGBAbuffer_index);
	return VertexArrayID;
}


GLuint task1b() {
	// sets up vertex xyz coordinates as floats ranging from -1 to 1, one vertex per row
	float vertices[] = {
		// Begin triforce
		0.0, -0.1, 0.0,  // x,y,z
		0.2, -0.1, 0.0,
		0.1,  0.1, 0.0,

		0.1, -0.3, 0.0,
		0.3, -0.3, 0.0,

		-0.1, -0.3, -0.0,
		// End triforce

		-0.2, 0.2, 0.0,
		0.2, 0.2, 0.0,
		0.1, 0.1, 0.0,
		0.0 , 0.3, 0.0
	};

	// sets up indices for vertices included in each triangle, one triangle per row
	int indices[] = {
		0, 1, 2,
		3, 4, 1,
		5, 3, 0,
		6, 7 ,9,
		7, 6, 8
	};

	// sets the global indicesSize for use when drawing
	indicesSize = sizeof(indices);
  float RGBAarray[] = {
    1, 1, 0,
    0, 1, 0,
    0.5, 0.5, 0.5,
    0, 0, 1,
    1, 1, 1
  };
	GLuint VAO;

	// Generates VAO from the vertices and indices
	VAO = setupVAO(vertices, sizeof(vertices), indices, sizeof(indices), RGBAarray);

	return VAO;
}
