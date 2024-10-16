#include "common.h"
#include <fstream>
#include <sstream>

// 파일 로딩용 함수
std::optional<std::string> LoadTextFile(const std::string &filename)
{
	std::ifstream fin(filename);
	if (!fin.is_open())
	{
		SPDLOG_ERROR("failed to open file: {}", filename);
		return {}; // optional file의 아무것도 없는 경우를 {}로 표기
	}
	std::stringstream text;
	text << fin.rdbuf();
	return text.str();
}