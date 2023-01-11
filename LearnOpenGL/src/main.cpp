#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "Shaders/Shader.h"
#include "Camera/Camera.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Camera
Camera ourCamera(glm::vec3(0.0f, 0.0f, 3.0f));

//timer
float deltaTime = 0.0f;
float lastTime = 0.0f;

//mouse
float lastX = SCR_HEIGHT / 2.f;
float lastY = SCR_WIDTH / 2.f;
bool firstMouse = true;

//light
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

unsigned int loadTexture(const std::string& path)
{
	//创建并绑定纹理
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//纹理环绕
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//指定边框外颜色
// 	float borderColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
// 	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//mipmap
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//读入纹理图片，填充宽度、高度、通道数
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	//生成纹理
	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture!" << std::endl;
	}
	//释放图像内存
	stbi_image_free(data);

	return texture;
}

int main()
{
	//初始化glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//设置回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//隐藏光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//注册了鼠标函数
	glfwSetCursorPosCallback(window, mouse_callback);
	//滚轮回调函数
	glfwSetScrollCallback(window, scroll_callback);

	//初始化glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("./src/shaders/shader.vs", "./src/shaders/shader.fs");

	Shader lightShader("./src/shaders/lightShader.vs", "./src/shaders/lightShader.fs");

	//顶点buffer
	float vertices[] = {
	// ---- 位置 ----    ----- 颜色 -----   -纹理坐标-     ----法线-----
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	 0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	 0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,	 0.0f, 0.0f, -1.0f,

 	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,	 0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	 0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	 0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,	 0.0f, 0.0f, 1.0f,

 	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	-1.0f, 0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	-1.0f, 0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	-1.0f, 0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	-1.0f, 0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,	-1.0f, 0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	-1.0f, 0.0f,  0.0f,

 	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 1.0f, 0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	 1.0f, 0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 1.0f, 0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 1.0f, 0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,	 1.0f, 0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 1.0f, 0.0f,  0.0f,

 	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 0.0f,-1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	 0.0f,-1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 0.0f,-1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 0.0f,-1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,	 0.0f,-1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 0.0f,-1.0f,  0.0f,

 	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 0.0f, 1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,	 0.0f, 1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 0.0f, 1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,	 0.0f, 1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,	 0.0f, 1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,	 0.0f, 1.0f,  0.0f
	};

	unsigned int indices[] = {
		// 注意索引从0开始! 
		// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
		// 这样可以由下标代表顶点组合成矩形

		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};

	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//创建VAO VBO顶点缓冲 EBO索引缓冲
	unsigned int VAO, VBO; // EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));//color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));//uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));//uv
	glEnableVertexAttribArray(3);

	//-------------------------------------------------------------
	//light--------------------------------------------------------
	//-------------------------------------------------------------

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//-------------------------------------------------------------
	//纹理---------------------------------------------------------
	// ------------------------------------------------------------
	
	//创建并绑定纹理
	unsigned int textureContainer = loadTexture("./assets/textures/container.jpg");
	unsigned int textureWall = loadTexture("./assets/textures/wall.jpg");
	unsigned int textureFace = loadTexture("./assets/textures/awesomeface.jpg");
	unsigned int textureKeqing = loadTexture("./assets/textures/keqing.png");
	unsigned int textureContainer2 = loadTexture("./assets/textures/container2.png");
	unsigned int textureContainer2Spec = loadTexture("./assets/textures/container2_specular.png");
	unsigned int textureMatrix = loadTexture("./assets/textures/matrix.jpg");
	//unsigned int textureContainer2Spec = loadTexture("./assets/textures/lighting_maps_specular_color.png");
	
	

	//-------------------------------------------------------------
	//shader-------------------------------------------------------
	// ------------------------------------------------------------
	
	//设置纹理采样器
	ourShader.use();
	

	//循环渲染
	//每次循环检查窗口是否退出
	while (!glfwWindowShouldClose(window))
	{
		//Esc 退出
		processInput(window);

		//timer
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		//渲染指令
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//VP
		glm::mat4 view = ourCamera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(ourCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		

		//cube绘制
		ourShader.use();

		//纹理槽位
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureContainer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureWall);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureFace);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureKeqing);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureContainer2);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, textureContainer2Spec);

		ourShader.setInt("textureContainer", 0);
		ourShader.setInt("textureWall", 1);
		ourShader.setInt("textureFace", 2);
		ourShader.setInt("textureKeqing", 3);
		ourShader.setInt("material.diffuse", 4);
		ourShader.setInt("material.specular", 5);

		ourShader.setVec3("cameraPos", ourCamera.Position);
		ourShader.setFloat("material.shininess", 20.0f);

		//平行光
		ourShader.setVec3("dirLight.direction",lightDirection);
		ourShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		ourShader.setVec3("dirLight.diffuse", 0.2f, 0.0f, 0.0f);
		ourShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);

		//点光
		// point light 1
		ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[0].diffuse", 0.2f, 0.0f, 0.0f);
		ourShader.setVec3("pointLights[0].specular", 0.2f, 0.2f, 0.2f);
		ourShader.setFloat("pointLights[0].constant", 1.0f);
		ourShader.setFloat("pointLights[0].linear", 0.09f);
		ourShader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[1].diffuse", 0.2f, 0.0f, 0.0f);
		ourShader.setVec3("pointLights[1].specular", 0.2f, 0.2f, 0.2f);
		ourShader.setFloat("pointLights[1].constant", 1.0f);
		ourShader.setFloat("pointLights[1].linear", 0.09f);
		ourShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[2].diffuse", 0.2f, 0.0f, 0.0f);
		ourShader.setVec3("pointLights[2].specular", 0.2f, 0.2f, 0.2f);
		ourShader.setFloat("pointLights[2].constant", 1.0f);
		ourShader.setFloat("pointLights[2].linear", 0.09f);
		ourShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[3].diffuse", 0.2f, 0.0f, 0.0f);
		ourShader.setVec3("pointLights[3].specular", 0.2f, 0.2f, 0.2f);
		ourShader.setFloat("pointLights[3].constant", 1.0f);
		ourShader.setFloat("pointLights[3].linear", 0.09f);
		ourShader.setFloat("pointLights[3].quadratic", 0.032f);

		//聚光
		ourShader.setVec3("spotLight.position", ourCamera.Position);
		ourShader.setVec3("spotLight.direction", ourCamera.Front);
		ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
		ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		ourShader.setVec3("spotLight.diffuse", 0.8f, 0.5f, 0.5f);
		ourShader.setVec3("spotLight.specular", 0.2f, 0.2f, 0.2f);

		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);

		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]) * glm::rotate(glm::mat4(1.0f), glm::radians( - i * 5.0f), glm::vec3(1.0f, 0.0f, 1.0f));
			ourShader.setMat4("model", model);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//光源绘制
		lightShader.use();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), lightPos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		lightShader.setVec3("lightColor", lightColor);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//交换buffer
		glfwSwapBuffers(window);
		//函数检查有没有触发时间：鼠标键盘、更新窗口等，并调用回调函数
		glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	//结束释放资源
	glfwTerminate();
	return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // 这个bool变量初始时是设定为true的
	{		
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	ourCamera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ourCamera.ProcessMouseScroll(yoffset);
}

// process all input: query GLFW whether relevant keys 
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		ourCamera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		ourCamera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		ourCamera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		ourCamera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}