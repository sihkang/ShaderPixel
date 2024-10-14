#include "texture.h"

TextureUPtr Texture::Create(int width, int height, uint32_t format, uint32_t type) {
  auto texture = TextureUPtr(new Texture());
  texture->CreateTexture();
  texture->SetTextureFormat(width, height, format, type);
  texture->SetFilter(GL_LINEAR, GL_LINEAR);
  return std::move(texture);
}

TextureUPtr Texture::CreateFromImage(const Image* image)
{
	auto texture = TextureUPtr(new Texture());
	texture->CreateTexture();
	texture->SetTextureFromImage(image);
	return std::move(texture);
}

Texture::~Texture()
{
	if (m_texture)
	{
		glDeleteTextures(1, &m_texture);
	}
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::CreateTexture()
{
	glGenTextures(1, &m_texture);
	// bind and set default filter and wrap option
	Bind();
	SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image *image)
{
	GLenum format = GL_RGBA;
	switch (image->GetChannelCount())
	{
		default: break;
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 3: format = GL_RGB; break;
	}

	m_width = image->GetWidth();
	m_height = image->GetHeight();
	m_format = format;
	m_type = GL_UNSIGNED_BYTE;
	
	glTexImage2D(GL_TEXTURE_2D, 0, m_format,
		m_width, m_height, 0,
		format, m_type,
		image->GetData());
		
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::SetTextureFormat(int width, int height, uint32_t format, uint32_t type) {
	m_width = width;
	m_height = height;
	m_format = format;
	m_type = type;

	GLenum imageFormat = GL_RGBA;
	if (m_format == GL_DEPTH_COMPONENT) 
	{
		imageFormat = GL_DEPTH_COMPONENT;        
	}
	else if (m_format == GL_RGB || m_format == GL_RGB16F || m_format == GL_RGB32F) 
	{
		imageFormat = GL_RGB;
	}
	else if (m_format == GL_RG || m_format == GL_RG16F || m_format == GL_RG32F) 
	{
		imageFormat = GL_RG;
	}
	else if (m_format == GL_RED || m_format == GL_R || m_format == GL_R16F || m_format == GL_R32F) 
	{
		imageFormat = GL_RED;
	}
 
	glTexImage2D(GL_TEXTURE_2D, 0, m_format,
		m_width, m_height, 0,
		imageFormat, m_type,
		nullptr);
}

CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*>& images) 
{
	auto texture = CubeTextureUPtr(new CubeTexture());
	if (!texture->InitFromImages(images))
		return nullptr;
	return std::move(texture);
}

CubeTexture::~CubeTexture() 
{
	if (m_texture) 
	{
		glDeleteTextures(1, &m_texture);
	}
}

void CubeTexture::Bind() const 
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);    // 위쪽 텍스쳐 클래스의 경우 바인딩 타겟이 GL_TEXTURE_2D였으나 여기선 CUBE_MAP.
	// CUBE_MAP 타겟이 따로 있다. 큐브맵에서는 이걸 이용.
}

void Texture::SetBorderColor(const glm::vec4& color) const 
{
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}

// 6 면의 이미지를 받아서 하나의 큐브 텍스쳐로 만들어서 6면에다 복사해서 텍스쳐를 초기화함.
bool CubeTexture::InitFromImages(const std::vector<Image*>& images) 
{
	glGenTextures(1, &m_texture);
	Bind();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 	// z축에 해당하는 좌표까지 고려해야해서 WRAP_R까지.

	for (uint32_t i = 0; i < (uint32_t)images.size(); i++) 
	{
		auto image = images[i];
		GLenum format = GL_RGBA;
		switch (image->GetChannelCount()) 
		{
			default: break;
			case 1: format = GL_RED; break;
			case 2: format = GL_RG; break;
			case 3: format = GL_RGB; break;
		}

		//GL_TEXTURE_CUBE_MAP_POSITIVE_X ~ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z target을 순차적으로 (i) 를 더해주면서 6면의 이미지 텍스쳐를 복사한다.
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			image->GetWidth(), image->GetHeight(), 0,
			format, GL_UNSIGNED_BYTE,
			image->GetData()
		);
	}

	return true;
}