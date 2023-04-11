#pragma once

#include <string>
#include "glad/glad.h"

class OpenGLTexture
{
public:

	OpenGLTexture(std::string filepath);
	~OpenGLTexture();

	unsigned int ID() { return m_ID; }
	
	void Bind();

private:

	unsigned int m_ID; 
	
	unsigned int m_Internal_format;
	unsigned int m_Image_format;
	unsigned int m_Wrap_S;
	unsigned int m_Wrap_T;
	unsigned int m_Filter_Min;
	unsigned int m_Filter_Mag; 

};