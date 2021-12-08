#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <iostream>
#include <learnopengl/shader.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/camera.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

typedef struct Material
{
	const char* Name;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;
} Material;

typedef struct ui_params
{
	Material material;
} ui_params;

const Material MaterialList[] =
{
	{ "emerald",{0.0215,0.1745,0.0215},{0.07568,0.61424,0.07568},{0.633,0.727811,0.633},0.6 },
	{ "jade",{0.135,0.2225,0.1575},{0.54,0.89,0.63},{0.316228,0.316228,0.316228},0.1 },
	{ "obsidian",{0.05375,0.05,0.06625},{0.18275,0.17,0.22525},{0.332741,0.328634,0.346435},0.3 },
	{ "pearl",{0.25,0.20725,0.20725},{1,0.829,0.829},{0.296648,0.296648,0.296648},0.088 },
	{ "ruby",{0.1745,0.01175,0.01175},{0.61424,0.04136,0.04136},{0.727811,0.626959,0.626959},0.6 },
	{ "turquoise",{0.1,0.18725,0.1745},{0.396,0.74151,0.69102},{0.297254,0.30829,0.306678},0.1 },
	{ "brass",{0.329412,0.223529,0.027451},{0.780392,0.568627,0.113725},{0.992157,0.941176,0.807843},0.21794872 },
	{ "bronze",{0.2125,0.1275,0.054},{0.714,0.4284,0.18144},{0.393548,0.271906,0.166721},0.2 },
	{ "chrome",{0.25,0.25,0.25},{0.4,0.4,0.4},{0.774597,0.774597,0.774597},0.6 },
	{ "copper",{0.19125,0.0735,0.0225},{0.7038,0.27048,0.0828},{0.256777,0.137622,0.086014},0.1 },
	{ "gold",{0.24725,0.1995,0.0745},{0.75164,0.60648,0.22648},{0.628281,0.555802,0.366065},0.4 },
	{ "silver",{0.19225,0.19225,0.19225},{0.50754,0.50754,0.50754},{0.508273,0.508273,0.508273},0.4 },
	{ "blackplastic",{0.0,0.0,0.0},{0.01,0.01,0.01},{0.50,0.50,0.50},0.25 },
	{ "cyanplastic",{0.0,0.1,0.06},{0.0,0.50980392,0.50980392},{0.50196078,0.50196078,0.50196078},0.25 },
	{ "greenplastic",{0.0,0.0,0.0},{0.1,0.35,0.1},{0.45,0.55,0.45},0.25 },
	{ "redplastic",{0.0,0.0,0.0},{0.5,0.0,0.0},{0.7,0.6,0.6},.25 },
	{ "whiteplastic",{0.0,0.0,0.0},{0.55,0.55,0.55},{0.70,0.70,0.70},.25 },
	{ "yellowplastic",{0.0,0.0,0.0},{0.5,0.5,0.0},{0.60,0.60,0.50},.25 },
	{ "blackrubber",{0.02,0.02,0.02},{0.01,0.01,0.01},{0.4,0.4,0.4},.078125 },
	{ "cyanrubber",{0.0,0.05,0.05},{0.4,0.5,0.5},{0.04,0.7,0.7},.078125 },
	{ "greenrubber",{0.0,0.05,0.0},{0.4,0.5,0.4},{0.04,0.7,0.04},.078125 },
	{ "redrubber",{0.05,0.0,0.0},{0.5,0.4,0.4},{0.7,0.04,0.04},.078125 },
	{ "whiterubber",{0.05,0.05,0.05},{0.5,0.5,0.5},{0.7,0.7,0.7},.078125 },
	{ "yellowrubber",{0.05,0.05,0.0},{0.5,0.5,0.4},{0.7,0.7,0.04},.078125 }
};

const char* MaterialNames[] =
{ "emerald","jade","obsidian","pearl","ruby","turquoise","brass","bronze","chrome","copper","gold","silver",
	"blackplastic","cyanplastic","greenplastic","redplastic","whiteplastic","yellowplastic","blackrubber",
	"cyanrubber","greenrubber","redrubber","whiterubber","yellowrubber" };


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* texPath, GLint format, bool flipY);

void imgui_on_init(GLFWwindow* window);
void imgui_on_render(ui_params& param);
void imgui_on_deinit(GLFWwindow* window);

static ui_params params;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH * 0.5f;
float lastY = SCR_HEIGHT * 0.5f;
static bool firstMouse = true;


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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	imgui_on_init(window);

	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// -----------------------------------------
	// vertex shader
	Shader lightingShader("3.1.basic_lighting.vs", "3.1.basic_lighting.fs");
	Shader lightCubeShader("3.1.light_cube.vs", "3.1.light_cube.fs");


	// set up vertex data
	float vertices[] = {
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	// VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// cube
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);
	// texCoord
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// light
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// light position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL); // No need to unbind EBO, GL_ELEMENT_ARRAY_BUFFER binding is part of the VAO state, it will automatically be unbound when VAO is unbound.
	//glBindVertexArray(NULL);	// No need to unbind VAO since VAO will be bound before it's used.
	glBindBuffer(GL_ARRAY_BUFFER, NULL);


	//Wireframe Mode: GL_FILL / GL_LINE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	lightingShader.use();
	lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);



	lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
	lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	glm::mat4 cubeModel = glm::mat4(1.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// Rendering Cube
		lightCubeShader.use();
		glm::mat4 lightModel = glm::mat4(1.0f);
		// lightModel = glm::rotate(lightModel, glm::radians(params.lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		lightCubeShader.setMat4("model", lightModel);

		glm::vec3 lightColor(1.0f);
		//lightColor.x = sin(glfwGetTime() * 2.0f);
		//lightColor.y = sin(glfwGetTime() * 0.7f);
		//lightColor.z = sin(glfwGetTime() * 1.3f);

		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // 降低影响
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影响

		lightingShader.setVec3("light.ambient", ambientColor);
		lightingShader.setVec3("light.diffuse", diffuseColor);


		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Rendering Light
		// use shader program
		lightingShader.use();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", cubeModel);
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setVec3("lightPos", lightModel[3]); // 4th column, Position Vec of Mat4

		lightingShader.setVec3("material.ambient", params.material.Ambient);
		lightingShader.setVec3("material.diffuse", params.material.Diffuse);
		lightingShader.setVec3("material.specular", params.material.Specular);
		lightingShader.setFloat("material.shininess", params.material.Shininess * 128.0f);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);



		imgui_on_render(params);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// free resources
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	imgui_on_deinit(window);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(window, NULL);
		glfwSetScrollCallback(window, NULL);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		firstMouse = true;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(const char* texPath, GLint format, bool flipY)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// setting wrap & filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(flipY);
	unsigned char* data = stbi_load(FileSystem::getPath(texPath).c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture.\n";
	}

	stbi_image_free(data);

	return texture;
}

void imgui_on_init(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}

void imgui_on_render(ui_params& param)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);

	static bool open = false;

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	if (!ImGui::Begin("Config", &open, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	static int item_idx = 0; // If the selection isn't within 0..count, Combo won't display a preview
	ImGui::Combo("Materials", &item_idx, MaterialNames, IM_ARRAYSIZE(MaterialNames));
	params.material = MaterialList[item_idx];

	ImGui::Separator();
	ImGui::ColorEdit4("ambient##2f", (float*)&params.material.Ambient, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit4("diffuse##2f", (float*)&params.material.Diffuse, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit4("specular##2f", (float*)&params.material.Specular, ImGuiColorEditFlags_Float);
	ImGui::Text("%.2f shininess", params.material.Shininess);

	ImGui::Text("Press 1 to show cursor");
	ImGui::Text("Press 2 to hide cursor");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void imgui_on_deinit(GLFWwindow* window)
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
