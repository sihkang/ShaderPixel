#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "program.h"
#include "shader.h"

CLASS_PTR(Context)
class Context {
public:
	static ContextUPtr Create();
	void Render();
	void ProcessInput(GLFWwindow* window);
	void Reshape(int width, int height);
	void MouseMove(double x, double y);
	void MouseButton(int button, int action, double x, double y);
	void UpdateCameraDirection();

	void DrawScene(const glm::mat4& view,
		const glm::mat4& projection,
		const Program* program);

private:
	Context() {}
	bool Init();
	
	ProgramUPtr m_program;

 	// screen size
	int m_width {640};
	int m_height {480};

	// camera parameter
	bool m_cameraControl { false };
	glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
	float m_cameraPitch { 0.0f };
	float m_cameraYaw { 0.0f };
	glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, 1.0f) };
	glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f,  -3.0f) };
	glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
	glm::mat3 m_cameraOrientation ;
};

#endif