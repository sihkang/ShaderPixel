#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

ImageUPtr Image::Load(const std::string& filepath)
{
	auto image = ImageUPtr(new Image());
	if (!image->LoadWithStb(filepath))
		return nullptr;
	return std::move(image);
}

Image::~Image()
{
	if (m_data)
		stbi_image_free(m_data);
}

ImageUPtr Image::Create(int width, int height, int channelCount)
{
	auto image = ImageUPtr(new Image());
	if (!image->Allocate(width, height, channelCount))
	{
		SPDLOG_INFO("Failed to allocate memory : {} * {} * {}", width, height, channelCount);
		return nullptr;
	}
	return std::move(image);
}

void Image::SetCheckImage(int gridX, int gridY)
{
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			int pos = (j * m_width + i) * m_channelCount;
			bool even = (((i / gridX) + (j / gridY)) % 2 == 0);
			uint8_t val = even ? 255 : 0;
			for (int k = 0; k < 3; k++)
				m_data[pos + k] = val;
		}
	}
}

bool Image::LoadWithStb(const std::string& filepath)
{
	m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
	if (!m_data)
	{
		SPDLOG_INFO("Failed to load image : {}", filepath);
		return false;
	}
	return true;
}

bool Image::Allocate(int width, int height, int channelCount)
{
	m_width = width;
	m_height = height;
	m_channelCount = channelCount;
	m_data = (uint8_t*)malloc(width * height * channelCount);
	if (!m_data)
	{
		SPDLOG_INFO("Failed to allocate memory : {} * {} * {}", width, height, channelCount);
		return false;
	}
	return true;
}