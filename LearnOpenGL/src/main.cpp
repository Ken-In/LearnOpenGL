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
	glDepthFunc(GL_LESS);//���ñȽϺ��� Ĭ��GL_LESS

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//Shader ourShader("./src/shaders/depthShader.vs", "./src/shaders/depthShader.fs");
	Shader ourShader("./src/Shaders/model_loading.vs", "./src/Shaders/model_loading.fs");
	Shader shaderSingleColor("./src/Shaders/shaderSingleColor.vs", "./src/Shaders/shaderSingleColor.fs");

	//Model ourModel("./assets/models/rock/rock.obj");
	//Model ourModel("./assets/models/backpack/backpack.obj");
	Model ourModel("./assets/models/nanosuit/nanosuit.obj");
	//Model ourModel("./assets/models/Ganyu/body.obj");

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

		shaderSingleColor.use();
		shaderSingleColor.setMat4("view", view);
		shaderSingleColor.setMat4("projection", projection);
		model *= glm::scale(glm::mat4(1.0f), glm::vec3(1.01f, 1.01f, 1.01f));
		shaderSingleColor.setMat4("model", model);

		//��model
		glStencilFunc(GL_ALWAYS, 1, 0xFF);// ���е�Ƭ�ζ�Ӧ�ø���ģ�建��
		glStencilMask(0xFF);// ����ģ�建��д��
		ourModel.Draw(ourShader);

		//���߿�
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);// ��������Ϊ1ʱ ͨ������
		glStencilMask(0x00);// ��ֹģ�建���д��
		//glDisable(GL_DEPTH_TEST);// ��ֹ��Ȳ���
		ourModel.Draw(shaderSingleColor);

		glBindVertexArray(0);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
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
	stbi_set_flip_vertically_on_load(false);
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