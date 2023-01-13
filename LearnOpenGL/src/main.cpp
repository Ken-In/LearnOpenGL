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

unsigned int loadTexture(const std::string& path);

int main()
{
	//��ʼ��glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//���ûص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//���ع��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//ע������꺯��
	glfwSetCursorPosCallback(window, mouse_callback);
	//���ֻص�����
	glfwSetScrollCallback(window, scroll_callback);

	//��ʼ��glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);//ִ����Ȳ��Բ�����Ƭ�Σ���������Ȼ���
	//glDepthFunc(GL_LESS);//���ñȽϺ��� Ĭ��GL_LESS

	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);//��˳ʱ����涨��Ϊ�����棺

	//Shader ourShader("./src/shaders/depthShader.vs", "./src/shaders/depthShader.fs");
	Shader ourShader("./src/Shaders/model_loading.vs", "./src/Shaders/model_loading.fs");
	Shader shaderSingleColor("./src/Shaders/shaderSingleColor.vs", "./src/Shaders/shaderSingleColor.fs");
	Shader blendShader("./src/Shaders/blendShader.vs", "./src/Shaders/blendShader.fs");
	
	//Model ourModel("./assets/models/rock/rock.obj");
	//Model ourModel("./assets/models/backpack/backpack.obj");
	Model ourModel("./assets/models/nanosuit/nanosuit.obj");
	//Model ourModel("./assets/models/Ganyu/body.obj");

	std::vector<glm::vec3> vegetation;
	vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
	vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));

	std::map<float, glm::vec3> sorted;
	for (int i = 0; i < vegetation.size(); i++)
	{
		float distance = vegetation[i].z;
		sorted[distance] = vegetation[i];
	}

	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int transparentVAO, transparentVBO;
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

	unsigned int transparentTexture = loadTexture("./assets/textures/grass.png");
	unsigned int windowTexture = loadTexture("./assets/textures/window.png");
	
	//ѭ����Ⱦ
	//ÿ��ѭ����鴰���Ƿ��˳�
	while (!glfwWindowShouldClose(window))
	{
		//Esc �˳�
		processInput(window);

		//timer
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;


		//��Ⱦָ��
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		ourShader.use();

		//VP
		glm::mat4 view = ourCamera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(ourCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		

		glm::mat4 model(1.0f);
		model *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		model *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		ourShader.setMat4("model", model);

		ourModel.Draw(ourShader);

		//alpha
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

		glBindVertexArray(0);
		glDisable(GL_BLEND);
		//����buffer
		glfwSwapBuffers(window);
		//���������û�д���ʱ�䣺�����̡����´��ڵȣ������ûص�����
		glfwPollEvents();
	}

	//�����ͷ���Դ
	glfwTerminate();
	return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // ���bool������ʼʱ���趨Ϊtrue��
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

unsigned int loadTexture(const std::string& path)
{
	//������������
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//��������ͼƬ������ȡ��߶ȡ�ͨ����
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	//��������
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
		//������
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		//�������
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture!" << std::endl;
	}
	//�ͷ�ͼ���ڴ�
	stbi_image_free(data);

	return textureID;
}