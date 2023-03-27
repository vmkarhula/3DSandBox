#include "OpenGLTexture.h"


#include <filesystem>
#include <iostream>

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using std::filesystem::path;

OpenGLTexture::OpenGLTexture(std::string filepath) : m_ID(0)
{
	path cppPath(filepath);

	if (!std::filesystem::exists(cppPath))
	{
		std::cerr << "Error! Texture file: " << cppPath << " does not exists" << std::endl;
		return;
	}

	int width, height, nrChannels;

	// OpenGL and stbi disagree with the upwards direction, this flips the texture vertically
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(cppPath.string().c_str(), &width, &height, &nrChannels, 0);

	GLuint imgFormat = 0;

	if(nrChannels == 3)
	{
		imgFormat = GL_RGB;
	}

	else if(nrChannels == 4)
	{
		imgFormat = GL_RGBA;
	}

	if (data)
	{
		glGenTextures(1, &m_ID);

		glBindTexture(GL_TEXTURE_2D, m_ID);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	else 
	{
		std::cerr << "Error! Could not load image data on image: " << cppPath << std::endl;
	}
}

OpenGLTexture::~OpenGLTexture()
{

}

void OpenGLTexture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}
