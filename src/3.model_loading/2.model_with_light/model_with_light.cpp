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
#include <learnopengl/model.h>

#include <stb_image.h>


// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

typedef struct ui_params
{
	glm::vec3 clearColor = glm::vec3(0.5f, 0.5f, 0.5f);

	int shininess = 32.0f;

	glm::vec3 dirLight_direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	glm::vec3 dirLight_ambient = glm::vec3(0.05f);
	glm::vec3 dirLight_diffuse = glm::vec3(0.4f);
	glm::vec3 dirLight_specular = glm::vec3(0.5f);

	glm::vec3 pointLight_ambient = glm::vec3(0.05f);
	glm::vec3 pointLight_diffuse = glm::vec3(0.8f);
	glm::vec3 pointLight_specular = glm::vec3(1.0f);

	float pointLight_constant = 1.0f;
	float pointLight_linear = 0.09f;
	float pointLight_quadratic = 0.032f;

	glm::vec3 spotLight_ambient = glm::vec3(0.0f);
	glm::vec3 spotLight_diffuse = glm::vec3(1.0f);
	glm::vec3 spotLight_specular = glm::vec3(1.0f);
	float spotLight_cutOff = glm::radians(12.5f);
	float spotLight_outerCutOff = glm::radians(15.0f);
} ui_params;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

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

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	imgui_on_init(window);

	glEnable(GL_DEPTH_TEST);
	
	//stbi_set_flip_vertically_on_load(true);
	
	glm::vec3 pointLightPositions[] =
	{
		glm::vec3(1.0f,  14.2f,  -5.0f),
		glm::vec3(0.0f, 14.3f, 1.7f),
		glm::vec3(-0.2f,  9.9f, 2.2f),
		glm::vec3(0.0f,  1.0f, 1.0f)
	};
	
	// vertex shader
	Shader ourShader("2.model_with_light.vs", "2.model_with_light.fs");

	// Models
	Model ourModel(FileSystem::getPath("res/objects/nanosuit/nanosuit.obj").c_str());

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	ourShader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

	//material
	ourShader.setFloat("material.shininess", params.shininess);

	// dirLight
	ourShader.setVec3("dirLight.direction", params.dirLight_direction);
	ourShader.setVec3("dirLight.ambient", params.dirLight_ambient);
	ourShader.setVec3("dirLight.diffuse", params.dirLight_diffuse);
	ourShader.setVec3("dirLight.specular", params.dirLight_specular);

	// pointLight
	for (int i = 0; i < 4; ++i)
	{
		char buf[128];

		sprintf(buf, "pointLights[%d].position", i);
		ourShader.setVec3(buf, pointLightPositions[i]);

		sprintf(buf, "pointLights[%d].constant", i);
		ourShader.setFloat(buf, params.pointLight_constant);

		sprintf(buf, "pointLights[%d].linear", i);
		ourShader.setFloat(buf, params.pointLight_linear);
		
		sprintf(buf, "pointLights[%d].quadratic", i);
		ourShader.setFloat(buf, params.pointLight_quadratic);

		sprintf(buf, "pointLights[%d].ambient", i);
		ourShader.setVec3(buf, params.pointLight_ambient);

		sprintf(buf, "pointLights[%d].diffuse", i);
		ourShader.setVec3(buf, params.pointLight_diffuse);

		sprintf(buf, "pointLights[%d].specular", i);
		ourShader.setVec3(buf, params.pointLight_specular);
	}

	// spotLight
	ourShader.setFloat("spotLight.cutOff", glm::cos(params.spotLight_cutOff));
	ourShader.setFloat("spotLight.outerCutOff", glm::cos(params.spotLight_outerCutOff));
	ourShader.setVec3("spotLight.ambient", params.spotLight_ambient);
	ourShader.setVec3("spotLight.diffuse", params.spotLight_diffuse);
	ourShader.setVec3("spotLight.specular", params.spotLight_specular);

	ourShader.setFloat("spotLight.constant", params.pointLight_constant);
	ourShader.setFloat("spotLight.linear", params.pointLight_linear);
	ourShader.setFloat("spotLight.quadratic", params.pointLight_quadratic);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// Rendering
		glClearColor(params.clearColor.r, params.clearColor.g, params.clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		ourShader.setMat4("model", model);
		ourShader.setVec3("viewPos", camera.Position);

		ourShader.setFloat("material.shininess", params.shininess);

		// directional light
		ourShader.setVec3("dirLight.ambient", params.dirLight_ambient);
		ourShader.setVec3("dirLight.diffuse", params.dirLight_diffuse);
		ourShader.setVec3("dirLight.specular", params.dirLight_specular);

		// point lights
		for (int i = 0; i < 4; ++i)
		{
			char buf[128];

			sprintf(buf, "pointLights[%d].ambient", i);
			ourShader.setVec3(buf, params.pointLight_ambient);

			sprintf(buf, "pointLights[%d].diffuse", i);
			ourShader.setVec3(buf, params.pointLight_diffuse);

			sprintf(buf, "pointLights[%d].specular", i);
			ourShader.setVec3(buf, params.pointLight_specular);
		}

		// spot light
		ourShader.setVec3("spotLight.ambient", params.spotLight_ambient);
		ourShader.setVec3("spotLight.diffuse", params.spotLight_diffuse);
		ourShader.setVec3("spotLight.specular", params.spotLight_specular);

		ourShader.setVec3("spotLight.position", camera.Position);
		ourShader.setVec3("spotLight.direction", camera.Front);
		ourShader.setFloat("spotLight.cutOff", glm::cos(params.spotLight_cutOff));
		ourShader.setFloat("spotLight.outerCutOff", glm::cos(params.spotLight_outerCutOff));

		ourModel.Draw(ourShader);
		
		// IMGUI rendering
		imgui_on_render(params);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// free resources

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
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
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

	//static bool show_demo_window = true;
	//if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);

	static bool open = false;

	//ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	if (!ImGui::Begin("Config", &open, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}


	ImGui::ColorEdit3("sky##1", (float*)&params.clearColor, ImGuiColorEditFlags_Float);
	ImGui::Separator();

	ImGui::Text("Material");
	ImGui::SliderInt("shininess", &params.shininess, 1, 128);
	ImGui::Separator();

	ImGui::Text("Directional Light:");
	ImGui::ColorEdit3("ambient##1", (float*)&params.dirLight_ambient, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("diffuse##1", (float*)&params.dirLight_diffuse, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("specular##1", (float*)&params.dirLight_specular, ImGuiColorEditFlags_Float);
	ImGui::Separator();

	ImGui::Text("Point Lights:");
	ImGui::ColorEdit3("ambient##2", (float*)&params.pointLight_ambient, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("diffuse##2", (float*)&params.pointLight_diffuse, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("specular##2", (float*)&params.pointLight_specular, ImGuiColorEditFlags_Float);
	ImGui::Separator();

	ImGui::Text("Spot Light:");
	ImGui::ColorEdit3("ambient##3", (float*)&params.spotLight_ambient, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("diffuse##3", (float*)&params.spotLight_diffuse, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("specular##3", (float*)&params.spotLight_specular, ImGuiColorEditFlags_Float);
	ImGui::SliderAngle("cutOff", &params.spotLight_cutOff, 5.0f, 15.0f);
	ImGui::SliderAngle("outerCutOff", &params.spotLight_outerCutOff, 6.0f, 20.0f);

	ImGui::Separator();
	ImGui::Text("camera.position:(%f,%f,%f)", camera.Position.x, camera.Position.y, camera.Position.z);
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
