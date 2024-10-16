#include "context.h"
#include <spdlog/spdlog.h>

ContextUPtr Context::Create()
{
	auto context = ContextUPtr(new Context());
	if (!context->Init())
		return nullptr;
	return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window)
{
	if (!m_cameraControl)
		return ;
	const float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * m_cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * m_cameraFront;
	
	auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * cameraRight;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * cameraRight;

	auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m_cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_cameraPos -= cameraSpeed * cameraUp;

	// SPDLOG_INFO("cameraPos: x({}), y({}), z({})", m_cameraPos[0], m_cameraPos[1], m_cameraPos[2]);
}

void Context::Reshape(int width, int height) 
{
	m_width = width;
	m_height = height;
	glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
	if (!m_cameraControl)
		return ;
	
	auto pos = glm::vec2((float)x, (float)y);
	auto deltaPos = pos - m_prevMousePos;

	const float cameraRotSpeed = 0.6f;
	m_cameraYaw -= deltaPos.x * cameraRotSpeed;
	m_cameraPitch -= deltaPos.y * cameraRotSpeed;

	if (m_cameraYaw < 0.0f) m_cameraYaw += 360.0f;
	if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

	if (m_cameraPitch > 89.0f) m_cameraPitch = 89.0f;
	if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

	m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			m_prevMousePos = glm::vec2((float)x, (float)y);
			m_cameraControl = true;
		}
		else if (action == GLFW_RELEASE)
		{
			m_cameraControl = false;
		}
	}
}

bool Context::Init()
{
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
	};
glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
	uint32_t vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint32_t vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	uint32_t ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	m_program = Program::Create("/Users/sihwan/Programming/shaderPixel/shader/test.vs", "/Users/sihwan/Programming/shaderPixel/shader/mandelbulb.fs");

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	auto model = glm::mat4(1.0f);
	m_cameraFront =
		glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
	m_cameraOrientation = glm::mat3(view);
	auto projection = glm::perspective<float>(glm::radians(60.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 40.0f);
	auto mvp = projection * view * model;
	
	m_program->Use();	

	glUniformMatrix4fv(glGetUniformLocation(m_program->Get(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));	
	glUniform3fv(glGetUniformLocation(m_program->Get(), "cameraPos"), 1, glm::value_ptr(m_cameraPos));
	glUniformMatrix3fv(glGetUniformLocation(m_program->Get(), "cameraOrientation"), 1, GL_FALSE, glm::value_ptr(m_cameraOrientation));
	return true;
}
void Context::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	auto model = glm::mat4(1.0f);
	m_cameraFront =
		glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
	m_cameraOrientation = glm::mat3(view);
	auto projection = glm::perspective<float>(glm::radians(45.0f), (float)m_width / (float)m_height, 1.0f, 40.0f);
	auto mvp = projection * view * model;
	
	glUniformMatrix4fv(glGetUniformLocation(m_program->Get(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform3fv(glGetUniformLocation(m_program->Get(), "cameraPos"), 1, glm::value_ptr(m_cameraPos));
	glUniformMatrix3fv(glGetUniformLocation(m_program->Get(), "cameraOrientation"), 1, GL_FALSE, glm::value_ptr(m_cameraOrientation));
	
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}