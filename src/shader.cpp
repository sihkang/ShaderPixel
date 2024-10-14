#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string& filename, GLenum shaderType)
{
	auto shader = ShaderUPtr(new Shader());
	if (!shader->LoadFile(filename, shaderType))
		return nullptr;
	return std::move(shader);
}

Shader::~Shader()
{
	if (m_shader)
	{
		glDeleteShader(m_shader);
	}
}

/**
 * LoadFile()
 * 파일 로딩 실패 시 false
 * 성공 시 로딩된 텍스트 포인터 및 길이 가져오기.
 * glCreateShader() 를 이용한 쉐이더 오브젝트 생성
 * glShaderSource()로 소스코드 입력
 * glCompileShader() 로 쉐이더 컴파일.
 * glGetShaderiv() 로 컴파일 상태 조회
 * 실패라면, glGetShaderInfoLog()로 에러 로그 가져오기
*/
bool Shader::LoadFile(const std::string &filename, GLenum shaderType)
{
	auto result = LoadTextFile(filename);
	if (!result.has_value()) // optional의 값이 있는지 없는지를 체크하는.
		return false;

	auto &code = result.value();
	const char * codePtr = code.c_str();
	int32_t codeLength = (int32_t)code.length();

	// create and compile shader
	m_shader = glCreateShader(shaderType);
	glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
	glCompileShader(m_shader);

	// check compile error
	int success = 0;
	glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[1024];
		glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
		SPDLOG_ERROR("failed to compile shader : \"{}\"", filename);
		SPDLOG_ERROR("reason: {}", infoLog);
		return false;
	}
	return true;
}