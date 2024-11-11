#include "context.h"
#include <spdlog/spdlog.h>
#include <imgui.h>



ContextUPtr Context::Create(std::string &shaderNum)
{
	auto context = ContextUPtr(new Context());
	if (!context->Init(shaderNum))
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
	// SPDLOG_INFO("cameraAngle: {}, {}", m_cameraYaw, m_cameraPitch);
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
	m_cameraYaw += deltaPos.x * cameraRotSpeed;
	m_cameraPitch -= deltaPos.y * cameraRotSpeed;

	if (m_cameraYaw < 0.0f) m_cameraYaw += 360.0f;
	if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

	if (m_cameraPitch > 89.0f) m_cameraPitch = 89.0f;
	if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

	m_prevMousePos = pos;
	SPDLOG_INFO("cameraFront: x({}), y({}), z({})", m_cameraFront[0], m_cameraPos[1], m_cameraPos[2]);
	// SPDLOG_INFO("cameraAngle: {}, {}", m_cameraYaw, m_cameraPitch);
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

bool Context::Init(std::string &shaderNum)
{
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
	};
	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

	glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
	if (shaderNum == "1")
		mandelBulb_setting(vertices, indices);
	if (shaderNum == "2")
		mandelBox_setting(vertices, indices);
	if (shaderNum == "3")
		transluscent_setting(vertices, indices);
	
	return true;
}

int Context::Render(std::string &shaderNum)
{
	if (shaderNum == "1")
		mandelbulb_render();
	else if (shaderNum == "2")
		mandelBox_render();
	else if (shaderNum == "3")
		transluscent_render();
	else
	{
		SPDLOG_INFO("error: unavailable Shader Number.");
		return -1;
	}
	return 0;
}

void Context::mandelBulb_setting(float *vertices, unsigned int *indices)
{
	// camera parameter
	m_cameraControl = false;
	m_prevMousePos = glm::vec2(0.0f);
	m_cameraPitch = 0.0f;
	m_cameraYaw = 0.0f;
	m_cameraPos = glm::vec3(0.0f, 0.0f,  1.0f);
	m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// light
	m_lightPos = glm::vec3(0.0f, 5.0f, 10.0f);

	vao = VertexLayout::Create();
	vbo = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 3 * 4);
	
	vao->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	ebo = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

	m_program = Program::Create("/Users/sihwan/Programming/shaderPixel/shader/basic.vs", "/Users/sihwan/Programming/shaderPixel/shader/mandelbulb.fs");

	auto model = glm::mat4(1.0f);
	m_cameraFront =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f),
			glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
	m_cameraOrientation = glm::mat3(view);
	auto projection = glm::perspective<float>(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 200.0f);
	auto mvp = projection * view * model;
	
	m_program->Use();	

	m_program->SetUniform("mvp", mvp);
	m_program->SetUniform("cameraPos", m_cameraPos);
	m_program->SetUniform("lightPos", m_lightPos);
	m_program->SetUniform("cameraTarget", m_cameraFront);
	m_program->SetUniform("iResolution", glm::vec2(m_width, m_height));
	
}

void Context::mandelbulb_render()
{
	static float time = 0.0f;
	if (ImGui::Begin("Camera Setting")) {
    	ImGui::SliderFloat("Camera Yaw", &m_cameraYaw, 0.0f, 360.0f); // 슬라이더 추가
    	ImGui::SliderFloat("Camera Pitch", &m_cameraPitch, -89.0f, 89.0f); // 슬라이더 추가

		// 카메라 Position 입력
		ImGui::InputFloat3("Camera Position", &m_cameraPos[0], "%.2f"); // 포맷을 통해 소수점 이하 두 자리까지 표현
		// 카메라 Front 벡터 입력
		ImGui::InputFloat3("Camera Front", &m_cameraFront[0], "%.2f");
		// 카메라 Up 벡터 입력
		ImGui::InputFloat3("Camera Up", &m_cameraUp[0], "%.2f");

		// float lightPos[3] = {m_lightPos[0], m_lightPos[1], m_lightPos[2]};
		ImGui::SliderFloat("lightPos1", &m_lightPos[0], -100.0f, 100.f);
		ImGui::SliderFloat("lightPos2", &m_lightPos[1], -100.0f, 100.f);
		ImGui::SliderFloat("lightPos3", &m_lightPos[2], -100.0f, 100.f);
	}
	
	ImGui::End();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	auto model = glm::mat4(1.0f);
	m_cameraFront =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f),
			glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
	m_cameraOrientation = glm::mat3(view);
	auto projection = glm::perspective<float>(glm::radians(45.0f), (float)m_width / (float)m_height, 1.0f, 100.0f);
	auto mvp = projection * view * model;
	
	m_program->SetUniform("mvp", mvp);
	m_program->SetUniform("cameraPos", m_cameraPos);
	m_program->SetUniform("lightPos", m_lightPos);
	m_program->SetUniform("cameraTarget", m_cameraFront);\
	m_program->SetUniform("iTime", time);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	time += 0.016;

}

void Context::mandelBox_setting(float * vertices, unsigned int * indices)
{

	// camera parameter
	m_cameraControl = false;
	m_prevMousePos = glm::vec2(0.0f);
	m_cameraPitch = 0.0f;
	m_cameraYaw = 0.0f;
	m_cameraPos = glm::vec3(1.0f, 1.0f, 10.0f);
	m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// light
	m_lightPos = glm::vec3(0.0f, 5.0f, 10.0f);

	vao = VertexLayout::Create();
	vbo = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 3 * 4);
	
	vao->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	ebo = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

	m_program = Program::Create("/Users/sihwan/Programming/shaderPixel/shader/basic.vs", "/Users/sihwan/Programming/shaderPixel/shader/mandelbox.fs");

	m_program->Use();	

	m_program->SetUniform("lightPos", m_lightPos);
	m_program->SetUniform("campos", m_cameraPos);
	m_program->SetUniform("camtar", m_cameraFront);
	m_program->SetUniform("iResolution", glm::vec2(m_width, m_height));
}

void Context::mandelBox_render()
{
	static float time = 0.0f;
	
	if (ImGui::Begin("Camera Setting")) {
		// 카메라 Position 입력
		ImGui::InputFloat3("Camera Position", &m_cameraPos[0], "%.2f"); // 포맷을 통해 소수점 이하 두 자리까지 표현
		// // 카메라 Front 벡터 입력
		// ImGui::InputFloat3("Camera Front", &m_cameraFront[0], "%.2f");
		// // 카메라 Up 벡터 입력
		// ImGui::InputFloat3("Camera Up", &m_cameraUp[0], "%.2f");
	}
	
	ImGui::End();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	m_program->SetUniform("lightPos", m_lightPos);
	m_program->SetUniform("iTime", time);
	m_program->SetUniform("campos", m_cameraPos);
	m_program->SetUniform("camtar", m_cameraFront);
	m_program->SetUniform("iResolution", glm::vec2(m_width, m_height));
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	time += 0.016;
}


void Context::transluscent_setting(float * vertices, unsigned int * indices)
{

	// camera parameter
	m_cameraControl = false;
	m_prevMousePos = glm::vec2(0.0f);
	m_cameraPitch = 0.0f;
	m_cameraYaw = 0.0f;
	m_cameraPos = glm::vec3(1.0f, 1.0f, 10.0f);
	m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// light
	m_lightPos = glm::vec3(0.0f, 5.0f, 10.0f);

	vao = VertexLayout::Create();
	vbo = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 3 * 4);
	
	vao->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	ebo = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);


	m_program = Program::Create("/Users/sihwan/Programming/shaderPixel/shader/basic.vs", "/Users/sihwan/Programming/shaderPixel/shader/texturetest.fs");

	m_program->Use();	

	m_program->SetUniform("lightPos", m_lightPos);
	m_program->SetUniform("campos", m_cameraPos);
	m_program->SetUniform("camtar", m_cameraFront);
	m_program->SetUniform("iResolution", glm::vec2(m_width, m_height));
}

void Context::transluscent_render()
{
	static float time = 1.0f;
	
	if (ImGui::Begin("Camera Setting")) {
		// 카메라 Position 입력
		ImGui::InputFloat3("Camera Position", &m_cameraPos[0], "%.2f"); // 포맷을 통해 소수점 이하 두 자리까지 표현
		// // 카메라 Front 벡터 입력
		// ImGui::InputFloat3("Camera Front", &m_cameraFront[0], "%.2f");
		// // 카메라 Up 벡터 입력
		// ImGui::InputFloat3("Camera Up", &m_cameraUp[0], "%.2f");
	}
	
	ImGui::End();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const siv::PerlinNoise::seed_type seed = 123456u + time;

	const siv::PerlinNoise perlin{ seed };
	
	auto image = Image::Create(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	for (int y = 0; y < image->GetHeight(); ++y)
	{
		for (int x = 0; x < image->GetWidth(); ++x)
		{
			const float noise = perlin.octave2D_01((x * 0.01), (y * 0.01), 4);	
		}
	}

	auto tex = Texture::CreateFromImage(image.get());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->Get());
	m_program->SetUniform("iChannel0", 0);

// glUniform1i(glGetUniformLocation(shaderProgram, "uNoiseTexture"), 0);
	m_program->SetUniform("lightPos", m_lightPos);
	m_program->SetUniform("iTime", time);
	m_program->SetUniform("campos", m_cameraPos);
	m_program->SetUniform("camtar", m_cameraFront);
	m_program->SetUniform("iResolution", glm::vec2(m_width, m_height));
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	time += 1.0;
}