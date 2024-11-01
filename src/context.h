#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "program.h"
#include "shader.h"
#include "buffer.h"
#include "vertexLayout.h"

CLASS_PTR(Context)
class Context {
public:
	static ContextUPtr Create(std::string &shaderNum);
	int Render(std::string &shaderNum);
	void ProcessInput(GLFWwindow* window);
	void Reshape(int width, int height);
	void MouseMove(double x, double y);
	void MouseButton(int button, int action, double x, double y);

	void DrawScene(const glm::mat4& view,
		const glm::mat4& projection,
		const Program* program);

	void mandelBulb_setting(float *vertices, unsigned int *indices);
	void mandelbulb_render();


	void mandelBox_setting(float *, unsigned int *);
	void mandelBox_render();

private:
	Context() {}
	bool Init(std::string &shaderNum);
	
	ProgramUPtr m_program;

 	// screen size
	int m_width {WINDOW_WIDTH};
	int m_height {WINDOW_HEIGHT};

	// Buffer
	VertexLayoutUPtr vao;
	BufferUPtr 		 vbo;
	BufferUPtr		 ebo;

	
	// camera parameter
	bool m_cameraControl { false };
	glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
	float m_cameraPitch { 0.0f };
	float m_cameraYaw { 0.0f };
	glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f,  1.0f) };
	glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };
	glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
	glm::mat3 m_cameraOrientation ;

	// light
	glm::vec3 m_lightPos { glm::vec3(0.0f, 5.0f, -1.0f) };
};

#endif