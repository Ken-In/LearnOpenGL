#include "Model/Model.h"
#include "Shaders/Shader.h"
#include "Camera/Camera.h"

#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLFWwindow* InitialzeWindow();
unsigned int loadTexture(const std::string& path);
unsigned int loadCubeMapTexture(const std::vector<std::string>& textures_faces);
void preVertexData();
void initialVAO();
void initialFrameBuffer();

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

//VAO VBO
unsigned int transparentVAO, transparentVBO;
unsigned int quadVAO, quadVBO;
unsigned int skyboxVAO, skyboxVBO;

//FrameBuffer
unsigned int framebuffer;
unsigned int texColorBuffer;
unsigned int rbo;

//uniform buffer
unsigned int uboMatrices;

//纹理
unsigned int transparentTexture;
unsigned int windowTexture;
unsigned int cubemapTexture;

//位移
std::vector<glm::vec3> vegetation;
std::map<float, glm::vec3> sorted;

//顶点数据
float transparentVertices[] = {
	// positions      //texCoords
 0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
 0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
 1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

 0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
 1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
 1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

//cubemap paths
std::vector<std::string> skyCubemap =
{
	"./assets/textures/skybox/right.jpg",
	"./assets/textures/skybox/left.jpg",
	"./assets/textures/skybox/top.jpg",
	"./assets/textures/skybox/bottom.jpg",
	"./assets/textures/skybox/front.jpg",
	"./assets/textures/skybox/back.jpg"
};
std::vector<std::string> otherCubemap =
{
	"./assets/textures/cubemaps/skyBox6/posx.jpg",
	"./assets/textures/cubemaps/skyBox6/negx.jpg",
	"./assets/textures/cubemaps/skyBox6/posy.jpg",
	"./assets/textures/cubemaps/skyBox6/negy.jpg",
	"./assets/textures/cubemaps/skyBox6/posz.jpg",
	"./assets/textures/cubemaps/skyBox6/negz.jpg",
};

int main()
{
	GLFWwindow* window = InitialzeWindow();
	if (!window)
		std::cout << "Failed to Initialize window!" << std::endl;

	preVertexData();
	initialVAO();
	initialFrameBuffer();

	//创建Shader
	//Shader ourShader("./src/shaders/depthShader.vs", "./src/shaders/depthShader.fs");
	Shader ourShader("./src/Shaders/reflectionShader.vs", "./src/Shaders/reflectionShader.fs");
	Shader shaderSingleColor("./src/Shaders/shaderSingleColor.vs", "./src/Shaders/shaderSingleColor.fs");
	Shader blendShader("./src/Shaders/blendShader.vs", "./src/Shaders/blendShader.fs");
	Shader frameBufferShader("./src/Shaders/frameBufferShader.vs", "./src/Shaders/frameBufferShader.fs");
	Shader skyboxShader("./src/Shaders/cubeMapShader.vs", "./src/Shaders/cubeMapShader.fs");

	//unifrom buffer shader绑定
	//只在ourshader里使用了uniform buffer
	unsigned int uniformBlockIndexModel = ourShader.GetUniformBlockIndex("Matrices");
	glUniformBlockBinding(ourShader.ID, uniformBlockIndexModel, 0);

	//填充ubo
	glm::mat4 projection = glm::perspective(glm::radians(ourCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	//创建Model
	//Model ourModel("./assets/models/rock/rock.obj");
	//Model ourModel("./assets/models/backpack/backpack.obj");
	Model ourModel("./assets/models/nanosuit_reflection/nanosuit.obj");
	//Model ourModel("./assets/models/Ganyu/body.obj");

	transparentTexture = loadTexture("./assets/textures/grass.png");
	windowTexture = loadTexture("./assets/textures/window.png");
	cubemapTexture = loadCubeMapTexture(skyCubemap);
	//cubemapTexture = loadCubeMapTexture(otherCubemap);

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
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glDepthMask(GL_FALSE);//执行深度测试并丢弃片段，不更新深度缓冲
		//glDepthFunc(GL_LESS);//设置比较函数 默认GL_LESS
		glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CW);//将顺时针的面定义为正向面
		glEnable(GL_PROGRAM_POINT_SIZE);//shader可以改变点pointsize

		glClearColor(.1f, .1f, .1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//VP
		glm::mat4 view = ourCamera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glm::mat4 projection = glm::perspective(glm::radians(ourCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		//-------------------------------------------------------------
		// model ------------------------------------------------------
		//-------------------------------------------------------------
		ourShader.use();
// 		ourShader.setMat4("view", view);
// 		ourShader.setMat4("projection", projection);
		ourShader.setVec3("cameraPos", ourCamera.Position);
		glActiveTexture(GL_TEXTURE5);
		ourShader.setInt("cubemap", 5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		glm::mat4 model(1.0f);
		model *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		model *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		ourShader.setMat4("model", model);

		ourModel.Draw(ourShader);

		//-------------------------------------------------------------
		// skyBox -----------------------------------------------------
		//-------------------------------------------------------------
		//渲染天空盒 不写入深度
		glDepthMask(GL_FALSE);
		skyboxShader.use();
		view = glm::mat4(glm::mat3(ourCamera.GetViewMatrix()));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		view = ourCamera.GetViewMatrix();

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);

		//-------------------------------------------------------------
		// blending ---------------------------------------------------
		//-------------------------------------------------------------
		//sorted
		
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		blendShader.use();
		blendShader.setMat4("view", view);
		blendShader.setMat4("projection", projection);

		glBindVertexArray(transparentVAO);
		glBindTexture(GL_TEXTURE_2D, windowTexture);
		for (auto it = sorted.begin(); it != sorted.end(); ++it)
		{
			model = glm::translate(glm::mat4(1.0f), it->second);
			blendShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//-------------------------------------------------------------
		// 后处理 -----------------------------------------------------
		//-------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		frameBufferShader.use();
		glBindVertexArray(quadVAO);
		frameBufferShader.setInt("screenTexture", 0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glDisable(GL_BLEND);
		//交换buffer
		glfwSwapBuffers(window);
		//函数检查有没有触发时间：鼠标键盘、更新窗口等，并调用回调函数
		glfwPollEvents();
	}

	//结束释放资源
	glfwTerminate();
	glDeleteFramebuffers(1, &framebuffer);

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

GLFWwindow* InitialzeWindow()
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
		return nullptr;
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
		return nullptr;
	}
	return window;
}

unsigned int loadTexture(const std::string& path)
{
	//创建并绑定纹理
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//读入纹理图片，填充宽度、高度、通道数
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	//生成纹理
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//纹理环绕
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		//纹理过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture!" << std::endl;
	}
	//释放图像内存
	stbi_image_free(data);

	return textureID;
}

unsigned int loadCubeMapTexture(const std::vector<std::string>& texture_faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < texture_faces.size(); i++)
	{
		data = stbi_load(texture_faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << texture_faces[i] << std::endl;
			stbi_image_free(data);
		}

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(1, 0);
	return textureID;
}

void preVertexData()
{
	//vegetation
	vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
	vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));

	//sorted
	for (int i = 0; i < vegetation.size(); i++)
	{
		float distance = vegetation[i].z;
		sorted[distance] = vegetation[i];
	}
}

void initialVAO()
{
	//透明VAO
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	//平面VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	//天空盒VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//uniform buffer
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);//分配内存
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//将ubo绑定到绑定点
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
}

void initialFrameBuffer()
{
	//帧缓冲
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// 生成纹理
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(1, 0);
	//附加纹理到帧缓冲
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//创建渲染对象
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//附加到帧缓冲
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

