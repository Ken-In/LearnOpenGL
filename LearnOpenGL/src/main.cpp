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
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//设置回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//初始化glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader ourShader("./src/shaders/shader.vs", "./src/shaders/shader.fs");
	


	//顶点buffer
	float vertices[] = {
	// ---- 位置 ----    ----- 颜色 -----   - 纹理坐标 -
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

	//创建VAO VBO顶点缓冲 EBO索引缓冲
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData把定义的数据复制到当前绑定缓冲区
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
	//纹理---------------------------------------------------------------
	// ------------------------------------------------------------------
	//创建并绑定纹理
	unsigned int textureContainer = loadTexture("./assets/textures/container.jpg");
	unsigned int textureWall = loadTexture("./assets/textures/wall.jpg");
	unsigned int textureFace = loadTexture("./assets/textures/awesomeface.jpg");
	unsigned int textureKeqing = loadTexture("./assets/textures/keqing.png");
	
	// 设置着色器采样器
	ourShader.use();
	ourShader.setInt("textureContainer", 0);
	ourShader.setInt("textureWall", 1);
	ourShader.setInt("textureFace", 2);
	ourShader.setInt("textureKeqing", 3);

	//循环渲染
	//每次循环检查窗口是否退出
	while (!glfwWindowShouldClose(window))
	{
		//Esc 退出
		processInput(window);

		//渲染指令
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

		//随时间变换矩阵
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
		ourShader.setMat4("transform", trans);

		//随时间变色
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

		//纹理混合值
		ourShader.setFloat("mixValue", mixValue);

		ourShader.use();
		glBindVertexArray(VAO);

		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			//只让3倍数箱子旋转
			if(i % 3 == 0)
			model *= rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.3, 0.5f));
			ourShader.setMat4("model", model);

			//图元类型，起始索引，绘制顶点数量
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//交换buffer
		glfwSwapBuffers(window);
		//函数检查有没有触发时间：鼠标键盘、更新窗口等，并调用回调函数
		glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	//结束释放资源
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