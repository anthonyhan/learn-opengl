#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <learnopengl/shader.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void checkShaderCompileStatus(unsigned int shader, bool isVertexShader);
void checkShaderLinkStatus(unsigned int program);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	// -----------------------------------------
	// vertex shader
	Shader shader("3.3.shader.vs", "3.3.shader.fs");


	// set up vertex data
	const float vertices[] = {
		//position						// Color
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // RightBottom
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // LeftBottom
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // Top
	};

	unsigned int indices[] = {
	0, 1, 2,
	};


	// VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	
	// 1. binding
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 2. set vertex attrib pointer
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);
	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	//unbind
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL); // No need to unbind EBO, GL_ELEMENT_ARRAY_BUFFER binding is part of the VAO state, it will automatically be unbound when VAO is unbound.
	//glBindVertexArray(NULL);	// No need to unbind VAO since VAO will be bound before it's used.
	glBindBuffer(GL_ARRAY_BUFFER, NULL); 


	//Wireframe Mode: GL_FILL / GL_LINE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// use shader program
		shader.use();

		// draw
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// free resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void checkShaderCompileStatus(unsigned int shader, bool isVertexShader)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::" << (isVertexShader ? "VERTEX" : "FRAGMENT") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void checkShaderLinkStatus(unsigned int program)
{
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
}
