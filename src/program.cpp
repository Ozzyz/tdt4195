// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

int indicesSize;
GLuint setupVAO(float*, int, int*, int);
GLuint task1b();
GLuint task1d();

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

  // get uniform value
  GLfloat value = -0.5;
  GLfloat increment = 0.01;

  // Rendering Loop
  while (!glfwWindowShouldClose(window))
  {
	// Clear colour and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw your scene here
	if (shader_is_activated){
		shader.activate();
	}
  value += increment;
  glUniform1f(2, value);
  if(value > 0.5 || value < -0.5){
    increment *= -1;
  }
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, (void*)0);

	if (shader_is_activated){shader.deactivate();}
      // Handle other events
      glfwPollEvents();
      // Flip buffers
      glfwSwapBuffers(window);
  }
	if (shader_is_activated){shader.destroy();}
}


void keyboardCallback(GLFWwindow* window, int key, int scancode,
                      int action, int mods)
{
    // Use escape key for terminating the GLFW window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
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
