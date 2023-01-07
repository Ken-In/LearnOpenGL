#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shaders/Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
		// 位置              // 颜色
		 0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // 右下
		 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // 左下
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, // 顶部
		-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f  // 左上角
	};

	unsigned int indices[] = {
		// 注意索引从0开始! 
		// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
		// 这样可以由下标代表顶点组合成矩形

		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//循环渲染
	//每次循环检查窗口是否退出
	while (!glfwWindowShouldClose(window))
	{
		//Esc退出
		processInput(window);

		//渲染指令
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//随时间变色
		float timeValue = (float)glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int timeColorLocation = ourShader.getUniform("timeColor");
		glUniform4f(timeColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


		ourShader.use();
		float offset = 0.5f;
		ourShader.setFloat("xOffset", offset);

		glBindVertexArray(VAO);
		//图元类型，起始索引，绘制顶点数量
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//函数检查有没有触发时间：鼠标键盘、更新窗口等，并调用回调函数
		glfwPollEvents();
		//交换buffer
		glfwSwapBuffers(window);
		
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}