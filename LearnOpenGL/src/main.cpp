#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shaders/Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;
float angle = -55.f;

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
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//���ûص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//��ʼ��glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader ourShader("./src/shaders/shader.vs", "./src/shaders/shader.fs");
	


	//����buffer
	float vertices[] = {
	// ---- λ�� ----    ----- ��ɫ -----   - �������� -
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
											
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
						 					
	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
						 					
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
											
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
						 					
	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  0.0f, 1.0f
	};

	unsigned int indices[] = {
		// ע��������0��ʼ! 
		// ����������(0,1,2,3)���Ƕ�������vertices���±꣬
		// �����������±��������ϳɾ���

		0, 1, 3, // ��һ��������
		1, 2, 3  // �ڶ���������
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

	//����VAO VBO���㻺�� EBO��������
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//���´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData�Ѷ�������ݸ��Ƶ���ǰ�󶨻�����
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//-------------------------------------------------------------------
	//����---------------------------------------------------------------
	// ------------------------------------------------------------------
	//������������
	unsigned int textureContainer = loadTexture("./assets/textures/container.jpg");
	unsigned int textureWall = loadTexture("./assets/textures/wall.jpg");
	unsigned int textureFace = loadTexture("./assets/textures/awesomeface.jpg");
	unsigned int textureKeqing = loadTexture("./assets/textures/keqing.png");
	
	// ������ɫ��������
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

		//��Ⱦָ��
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//MVP
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);

		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 1.0f, 0.0f));
		view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		ourShader.setMat4("model", model);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);

		//��ʱ��任����
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
		ourShader.setMat4("transform", trans);

		//��ʱ���ɫ
		float timeValue = (float)glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		ourShader.setFloat4("timeColor", 0.0f, greenValue, 0.0f, 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureContainer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureWall);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureFace);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureKeqing);

		//������ֵ
		ourShader.setFloat("mixValue", mixValue);

		ourShader.use();
		glBindVertexArray(VAO);

		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			//ֻ��3����������ת
			if(i % 3 == 0)
			model *= rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.3, 0.5f));
			ourShader.setMat4("model", model);

			//ͼԪ���ͣ���ʼ���������ƶ�������
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//����buffer
		glfwSwapBuffers(window);
		//���������û�д���ʱ�䣺�����̡����´��ڵȣ������ûص�����
		glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	//�����ͷ���Դ
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys 
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.0003f;
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.0003f;
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		angle -= 0.05f;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		angle += 0.05f;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}