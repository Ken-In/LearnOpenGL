#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

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
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

unsigned int loadTexture(const std::string& path)
{
	//������������
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//ָ���߿�����ɫ
// 	float borderColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
// 	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//�������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//mipmap
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//��������ͼƬ������ȡ��߶ȡ�ͨ����
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	//��������
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
	//�ͷ�ͼ���ڴ�
	stbi_image_free(data);

	return texture;
}

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

	Shader ourShader("./src/shaders/shader.vs", "./src/shaders/shader.fs");

	Shader lightShader("./src/shaders/lightShader.vs", "./src/shaders/lightShader.fs");

	//����buffer
	float vertices[] = {
	// ---- λ�� ----    ----- ��ɫ -----   -��������-     ----����-----
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
		// ע��������0��ʼ! 
		// ����������(0,1,2,3)���Ƕ�������vertices���±꣬
		// �����������±��������ϳɾ���

		0, 1, 3, // ��һ��������
		1, 2, 3  // �ڶ���������
	};

	//����VAO VBO���㻺�� EBO��������
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
	//����---------------------------------------------------------
	// ------------------------------------------------------------
	
	//������������
	unsigned int textureContainer = loadTexture("./assets/textures/container.jpg");
	unsigned int textureWall = loadTexture("./assets/textures/wall.jpg");
	unsigned int textureFace = loadTexture("./assets/textures/awesomeface.jpg");
	unsigned int textureKeqing = loadTexture("./assets/textures/keqing.png");
	
	//-------------------------------------------------------------
	//shader-------------------------------------------------------
	// ------------------------------------------------------------
	
	//�������������
	ourShader.use();
	ourShader.setInt("textureContainer", 0);
	ourShader.setInt("textureWall", 1);
	ourShader.setInt("textureFace", 2);
	ourShader.setInt("textureKeqing", 3);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//VP
		glm::mat4 view = ourCamera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(ourCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		//�����λ
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureContainer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureWall);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureFace);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureKeqing);

		//cube����
		ourShader.use();
		ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("lightColor", lightColor);
		ourShader.setVec3("lightPos", lightPos);
		ourShader.setVec3("cameraPos", ourCamera.Position);

		ourShader.setVec3("material.ambient", 0.0f, 0.1f, 0.06f);
		ourShader.setVec3("material.diffuse", 0.0f, 0.50980392f, 0.50980392f);
		ourShader.setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
		ourShader.setFloat("material.shininess", 32.0f);

		ourShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
		ourShader.setVec3("light.diffuse", lightColor);
		ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		glm::mat4 model(1.0f);
		ourShader.setMat4("model", model);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//��Դ����
		lightShader.use();
		model = glm::translate(glm::mat4(1.0f), lightPos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		lightShader.setVec3("lightColor", lightColor);


		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//����buffer
		glfwSwapBuffers(window);
		//���������û�д���ʱ�䣺�����̡����´��ڵȣ������ûص�����
		glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

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